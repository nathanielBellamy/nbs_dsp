#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <portaudio.h>
#include "pa.h"
#include "pa_data.h"

// following: https://github.com/PortAudio/portaudio/blob/master/examples/pa_fuzz.c

#define PA_SAMPLE_TYPE      paFloat32

typedef float SAMPLE;

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
  PA_DATA paData_initial = *paData;

  if( paData->buffer == NULL )
  {
      for( i=0; i<framesPerBuffer; i++ )
      {
          *out++ = 0;  /* left - silent */
          *out++ = 0;  /* right - silent */
      }
  }
  else if (paData->index >= paData->sfinfo.frames) 
  {
    paData->index = 0;
    return paComplete;
  }
  else
  {
    // process
    //
    // paData->buffer --copy--> paData->fft_buffer
    for (i = 0; i < framesPerBuffer; i++) {
      for (int ch = 0; ch < paData->sfinfo.channels; ch++) {
        *paData->fft_buffer++ = (paData->buffer[i * paData->sfinfo.channels + ch]);
      }
      paData->fft_buffer = paData_initial.fft_buffer; // reset pointer to buffer start
    }

    // paData->fft_buffer --for each channel, copy--> paData->fft_time
    for (int ch = 0; ch < paData->sfinfo.channels; ch++) {
      // copy channel into paData->fft_time
      for (i = 0; i < framesPerBuffer; i++) {
        paData->fft_time[i] = paData->buffer[i * paData->sfinfo.channels + ch];
      }

      // compute fft
      fftwf_execute(paData->fft_plan_to_freq);

      // mutate paData->fft_freq in place
      for (i = 0; i < framesPerBuffer; i++) {
        if (i > 30)// TODO: unmagic frequency cutoff
        {
          *paData->fft_freq[i] = 0.0f;
        }
      }

      // transform back into time domain
      fftwf_execute(paData->fft_plan_to_time);

      // normalize fft_time 
      for (i = 0; i < framesPerBuffer; i++) {
        paData->fft_time[i] = paData->fft_time[i] / framesPerBuffer;
      }
      
      // write out to paData->fft_buffer
      for (i = 0; i < framesPerBuffer; i++) {
        paData->buffer[i * paData->sfinfo.channels + ch] = paData->fft_time[i];
      }
    }
    
    // paData->fft_buffer --copy--> paOut
    for (i = 0; i < framesPerBuffer; i++) {
      for (int ch = 0; ch < paData->sfinfo.channels; ch++) {
        *out++ = paData->fft_buffer[paData->index * paData->sfinfo.channels + ch];
      }
      paData->index++;
    }
  }

  // printf("idx: %lli \n", paData->index);
  return paContinue;
}

int pa(PA_DATA *paData)
{
  PaStreamParameters inputParameters, outputParameters;
  PaStream *stream;
  PaError err;
  
  printf("pa_samplerate: %i \n", paData->sfinfo.samplerate);
  printf("pa_buff_frames: %lli \n", paData->buffer_frames);
  printf("pa_format: %i \n", paData->sfinfo.format);

  err = Pa_Initialize();
  if( err != paNoError ) goto error;

  inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
  if (inputParameters.device == paNoDevice) {
      fprintf(stderr,"Error: No default input device.\n");
      goto error;
  }
  inputParameters.channelCount = 2;       /* stereo in from file */
  inputParameters.sampleFormat = PA_SAMPLE_TYPE;
  inputParameters.hostApiSpecificStreamInfo = NULL;

  outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
  if (outputParameters.device == paNoDevice) {
      fprintf(stderr,"Error: No default output device.\n");
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

  printf("Hit ENTER to stop program.\n");
  char c;
  while( (c = getchar()) != '\n' && c != EOF ) 
  {
    if( !Pa_IsStreamActive(stream) )
    {
      goto handlePaComplete;
    }
  }
  goto cleanup;

  handlePaComplete:
    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;
    goto cleanup;

  cleanup:
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;
    printf("pa_idx_2: %lli \n", paData->index);

    Pa_Terminate();
    return 0;

  error:
    Pa_Terminate();
    fprintf( stderr, "An error occurred while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return -1;
}
