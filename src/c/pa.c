#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sndfile.h>
#include <portaudio.h>
#include <complex.h>
#include <fftw3.h>
#include "pa.h"
#include "pa_data.h"

// following: https://github.com/PortAudio/portaudio/blob/master/examples/pa_fuzz.c

#define PA_SAMPLE_TYPE      paFloat32

typedef float SAMPLE;

void freePaData(PA_DATA *paData) {
  printf("\nCleaning up resources...");
  
  free(paData->buffer);
  fftwf_free(paData->fft_buffer);
  fftwf_free(paData->fft_time);
  fftwf_free(paData->fft_freq);
  fftwf_destroy_plan(paData->fft_plan_to_freq);
  fftwf_destroy_plan(paData->fft_plan_to_time);
  sf_close(paData->file);
  
  printf("\nDone.");
};

int init_pa(PA_DATA *paData)
{
  paData->index = 0;
  paData->buffer_frames = 32;
  printf("\npa_idx_start: %lli", paData->index);

  if (! (paData->file = sf_open("gtfam_mini.wav", SFM_READ, &paData->sfinfo)))
  {
		printf ("Not able to open input file.\n") ;
		/* Print the error message from libsndfile. */
		puts (sf_strerror (NULL)) ;
    return 1 ;
  };
  
  // Display some information about the file.
  printf("\nSample rate: %d", paData->sfinfo.samplerate);
  printf("\nChannels: %d", paData->sfinfo.channels);
  printf("\nFrames: %lli", paData->sfinfo.frames);
  
  // Allocate memory for data
  paData->buffer = (float *) malloc(paData->sfinfo.frames * paData->sfinfo.channels * sizeof(float));
  if (!paData->buffer) {
      printf("\nCannot allocate memory");
      return 1;
  }

  // Read the audio data into buffer
  long readcount = sf_read_float(paData->file, paData->buffer, paData->sfinfo.frames * paData->sfinfo.channels);
  
  printf("\nreadcount: %ld", readcount);

  // allocate memory to compute fast fourier transform in pa_callback
  paData->fft_buffer = (float*) fftwf_malloc(sizeof(float) * paData->buffer_frames * paData->sfinfo.channels);
  paData->fft_time = (float*) fftwf_malloc(sizeof(float) * paData->buffer_frames);
  paData->fft_freq = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * paData->buffer_frames);
  paData->fft_plan_to_freq = fftwf_plan_dft_r2c_1d(
    paData->buffer_frames, 
    paData->fft_time,
    paData->fft_freq, 
    FFTW_ESTIMATE
  );
  paData->fft_plan_to_time = fftwf_plan_dft_c2r_1d(
    paData->buffer_frames, 
    paData->fft_freq, 
    paData->fft_time,
    FFTW_ESTIMATE
  );
  
  return 0;
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
  PA_DATA *paData = (PA_DATA *) userData;
  // PA_DATA paData_initial = *paData;

  if( paData->buffer == NULL )
  {
      for( i=0; i<framesPerBuffer; i++ )
      {
          *out++ = 0;  /* left - silent */
          *out++ = 0;  /* right - silent */
      }
  }
  else if (paData->index > paData->sfinfo.frames - 1) 
  {
    // paData->index = 0;
    // return paComplete;
    return 1;
  }
  else
  {
    // process
    //
    // paData->buffer --copy--> paData->fft_buffer
    for (i = 0; i < framesPerBuffer * paData->sfinfo.channels; i++) {
      paData->fft_buffer[i] = paData->buffer[paData->index + i];
    }

    // render fft for each channel
    for (int ch = 0; ch < paData->sfinfo.channels; ch++) {
      // copy channel into paData->fft_time
      for (i = 0; i < framesPerBuffer; i++) {
        paData->fft_time[i] = paData->fft_buffer[i * paData->sfinfo.channels + ch];
      }

      // compute fft
      fftwf_execute(paData->fft_plan_to_freq);

      // mutate paData->fft_freq in place
      // for (i = 0; i < 16; i++) {
      //   paData->fft_freq[2*i] = paData->fft_freq[2*i] * 0.2f + paData->fft_freq[2*i + 1] * -0.3f;
      //   paData->fft_freq[2*i +1] = paData->fft_freq[2*i] * -0.2f + paData->fft_freq[2*i + 1] * 0.1f;
      // }

      // transform back into time domain
      fftwf_execute(paData->fft_plan_to_time);

      // normalize fft_time 
      // write out to paData->fft_buffer
      for (i = 0; i < framesPerBuffer; i++) {
        paData->fft_buffer[i * paData->sfinfo.channels + ch] = paData->fft_time[i] / 32.0f;
      }
    }
    
    // paData->fft_buffer --copy--> paOut
    for (i = 0; i < framesPerBuffer; i++) {
      for (int ch = 0; ch < paData->sfinfo.channels; ch++) {
        *out++ = paData->fft_buffer[i * paData->sfinfo.channels + ch];
        paData->index++;
      }
    }
  }

  // printf("idx: %lli \n", paData->index);
  return paContinue;
}

void *pa(void *paData_)
{
  PA_DATA *paData = (PA_DATA*)paData_;
  PaStreamParameters inputParameters, outputParameters;
  PaStream *stream;
  PaError err;
  
  printf("\npa_samplerate: %i", paData->sfinfo.samplerate);
  printf("\npa_buff_frames: %lli", paData->buffer_frames);
  printf("\npa_format: %i", paData->sfinfo.format);

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
            paData->sfinfo.samplerate,
            paData->buffer_frames,
            paNoFlag, /* paClipOn, */   
            callback,
            paData );
  if( err != paNoError ) goto error;

  err = Pa_StartStream( stream );
  if( err != paNoError ) goto error;

  printf("\nHit ENTER to stop program.");
  char c;
  while( ( c = getchar() ) != '\n' && c != EOF ) {}
  goto handlePaComplete;

  handlePaComplete:
    printf("\nhandlePaComplete");
    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;
    goto cleanup;

  cleanup:
    printf("\npa cleanup");
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;
    printf("\npa_idx_end: %lli", paData->index);

    Pa_Terminate();
    return NULL;

  error:
    Pa_Terminate();
    fprintf( stderr, "\nAn error occurred while using the portaudio stream" );
    fprintf( stderr, "\nError number: %d", err );
    fprintf( stderr, "\nError message: %s", Pa_GetErrorText( err ) );
    return NULL;
}
