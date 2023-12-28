#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <math.h>
#include <sndfile.h>
#include <portaudio.h>
#include <pthread.h>
#include <complex.h>
#include <fftw3.h>
#include "audio.h"
#include "audio_data.h"

// following: https://github.com/PortAudio/portaudio/blob/master/examples/pa_fuzz.c

#define PA_SAMPLE_TYPE      paFloat32

typedef float SAMPLE;

void freeAudioData(AUDIO_DATA *audioData) {
  // printf("\nCleaning up resources...");
  
  free(audioData->buffer);
  fftwf_free(audioData->fft_buffer);
  fftwf_free(audioData->fft_time);
  fftwf_free(audioData->fft_freq);
  fftwf_destroy_plan(audioData->fft_plan_to_freq);
  fftwf_destroy_plan(audioData->fft_plan_to_time);
  sf_close(audioData->file);
  
  // printf("\nDone.");
};

int init_pa(AUDIO_DATA *audioData, atomic_int *atomicCounter, atomic_int *debugInt)
{
  audioData->atomicCounter = atomicCounter;
  audioData->debugInt = debugInt;
  audioData->index = 0;
  audioData->buffer_frames = 32;
  audioData->buffer_frames_d2p1 = 17;

  // https://svn.ict.usc.edu/svn_vh_public/trunk/lib/vhcl/libsndfile/doc/api.html
  // > When opening a file for read, the format field should be set to zero before calling sf_open().
  audioData->sfinfo.format = 0;

  if (! (audioData->file = sf_open("clr.wav", SFM_READ, &audioData->sfinfo)))
  {
		printf ("Not able to open input file.\n") ;
		/* Print the error message from libsndfile. */
		puts (sf_strerror (NULL)) ;
    return 1 ;
  };
  
  // Allocate memory for data
  audioData->buffer = (float *) malloc(audioData->sfinfo.frames * audioData->sfinfo.channels * sizeof(float));
  if (!audioData->buffer) {
      printf("\nCannot allocate memory");
      return 1;
  }

  // Read the audio data into buffer
  long readcount = sf_read_float(audioData->file, audioData->buffer, audioData->sfinfo.frames * audioData->sfinfo.channels);
  if (readcount == 0) {
      printf("\nCannot read file");
      return 1;
  }
  
  // allocate memory to compute fast fourier transform in pa_callback
  audioData->fft_buffer = (float*) fftwf_malloc(sizeof(float) * audioData->buffer_frames * audioData->sfinfo.channels);
  audioData->fft_time = (float*) fftwf_malloc(sizeof(float) * audioData->buffer_frames);
  audioData->fft_freq = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * audioData->buffer_frames_d2p1);
  audioData->fft_plan_to_freq = fftwf_plan_dft_r2c_1d(
    audioData->buffer_frames, 
    audioData->fft_time,
    audioData->fft_freq, 
    FFTW_ESTIMATE
  );
  audioData->fft_plan_to_time = fftwf_plan_dft_c2r_1d(
    audioData->buffer_frames, 
    audioData->fft_freq, 
    audioData->fft_time,
    FFTW_ESTIMATE
  );
  
  return 0;
}

double magnitude(fftwf_complex* z) 
{
    return sqrt( z[0]*z[0] + z[1]*z[1] );
}

static int callback(const void *inputBuffer, void *outputBuffer,
                    unsigned long framesPerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,
                    PaStreamCallbackFlags statusFlags,
                    void *userData )
{
  SAMPLE *out = (SAMPLE*)outputBuffer;
  unsigned int i;
  (void) inputBuffer;
  (void) timeInfo; /* Prevent unused variable warnings. */
  (void) statusFlags;
  AUDIO_DATA *audioData = (AUDIO_DATA *) userData;
  // AUDIO_DATA audioData_initial = *audioData;
  atomic_fetch_add(audioData->atomicCounter, framesPerBuffer);

  if( audioData->buffer == NULL )
  {
      for( i=0; i < framesPerBuffer; i++ )
      {
          *out++ = 0;  /* left - silent */
          *out++ = 0;  /* right - silent */
      }
  }
  else if (audioData->index > audioData->sfinfo.frames * audioData->sfinfo.channels + 1) 
  {
    // audioData->index = 0;
    // return paComplete;
    return 1;
  }
  else
  {
    // process
    //
    // audioData->buffer --copy--> audioData->fft_buffer
    for (i = 0; i < framesPerBuffer * audioData->sfinfo.channels; i++) {
      audioData->fft_buffer[i] = audioData->buffer[audioData->index + i];
    }

    // render fft for each channel
    for (int ch = 0; ch < audioData->sfinfo.channels; ch++) {
      // copy channel into audioData->fft_time
      for (i = 0; i < framesPerBuffer; i++) {
        audioData->fft_time[i] = audioData->fft_buffer[i * audioData->sfinfo.channels + ch];
      }

      // compute fft
      fftwf_execute(audioData->fft_plan_to_freq);

      // - we control neither when nor how frequently this method is called
      //   and we cannot perform any blocking operations here (ie. no mutexs)
      // - so we need a method of ensuring that, when the visual thread loads the values
      //   in atomicEQ, it loads values from a single application of the DFT
      // - we use the atomicEqSync variable to achieve this
      
      int eqSync = atomic_load( audioData->atomicEqSync );
      if ( eqSync == 1 )
      { // visual thread is ready to receive data
        // write data to be read by visual thread
        for (i = 0; i < audioData->buffer_frames_d2p1; i++)
        {
          atomic_store(
            audioData->atomicEQ + ( i + ( ch * audioData->buffer_frames_d2p1 ) ), 
            magnitude( audioData->fft_freq + i ) * 10000.0 // - atomic_store truncates the double to form an int here
                                                          //   we multiply by a factor here to maintain data
                                                          //   when the float is truncated to an int
                                                          //   but we will normalize these values by the largest amongst them
                                                          //   so this factor will cancel out
          );
        }
        
        // - disable write
        // - visual thread will enable when ready
        if ( ch == 1 ) // write all channels 
        {
          atomic_store(
            audioData->atomicEqSync,
            0
          );
        }
      }

      // transform back into time domain
      fftwf_execute(audioData->fft_plan_to_time);

      // normalize fft_time 
      // write out to audioData->fft_buffer
      for (i = 0; i < framesPerBuffer; i++) {
        audioData->fft_buffer[i * audioData->sfinfo.channels + ch] = audioData->fft_time[i] / 32.0f;
      }
    }
    
    // audioData->fft_buffer --copy--> paOut
    for (i = 0; i < framesPerBuffer; i++) {
      for (int ch = 0; ch < audioData->sfinfo.channels; ch++) {
        *out++ = audioData->fft_buffer[i * audioData->sfinfo.channels + ch];
        audioData->index++;
      }
    }
  }

  return paContinue;
}

void *audioMain(void *audioData_)
{
  AUDIO_DATA *audioData = (AUDIO_DATA*)audioData_;
  PaStreamParameters inputParameters, outputParameters;
  PaStream *stream;
  PaError err;

  err = Pa_Initialize();
  if( err != paNoError ) goto error;

  inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
  if (inputParameters.device == paNoDevice) {
      fprintf(stderr,"\nError: No default input device.");
      goto error;
  }
  inputParameters.channelCount = 2;       /* stereo in from file */
  inputParameters.sampleFormat = PA_SAMPLE_TYPE;
  inputParameters.hostApiSpecificStreamInfo = NULL;

  outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
  if (outputParameters.device == paNoDevice) {
      fprintf(stderr,"\nError: No default output device.");
      goto error;
  }
  outputParameters.channelCount = 2;       /* stereo output */
  outputParameters.sampleFormat = PA_SAMPLE_TYPE;
  outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;
  
  err = Pa_OpenStream(
            &stream,
            &inputParameters,
            &outputParameters,
            audioData->sfinfo.samplerate,
            audioData->buffer_frames,
            paNoFlag, /* paClipOn, */   
            callback,
            audioData );
  if( err != paNoError ) goto error;

  err = Pa_StartStream( stream );
  if( err != paNoError ) goto error;

  char c;
  while( ( c = getchar() ) != '\n' && c != EOF ) {}
  err = Pa_StopStream( stream );
  if( err != paNoError ) goto error;

  err = Pa_CloseStream( stream );
  if( err != paNoError ) goto error;
  Pa_Terminate();
  pthread_exit((void *) 0);

  error:
    Pa_Terminate();
    fprintf( stderr, "\nAn error occurred while using the portaudio stream" );
    fprintf( stderr, "\nError number: %d", err );
    fprintf( stderr, "\nError message: %s", Pa_GetErrorText( err ) );
    pthread_exit((void *) 1);
}
