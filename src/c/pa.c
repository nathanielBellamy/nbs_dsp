#include <stdio.h>
#include <portaudio.h>
#include "pa.h"
#include "pa_data.h"

// following: https://github.com/PortAudio/portaudio/blob/master/examples/pa_fuzz.c

#define PA_SAMPLE_TYPE      paFloat32
#define FRAMES_PER_BUFFER   (64)

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

  if( paData->buffer == NULL )
  {
      for( i=0; i<framesPerBuffer; i++ )
      {
          *out++ = 0;  /* left - silent */
          *out++ = 0;  /* right - silent */
      }
  }
  // else if (paData->index >= paData->buffer_frames) 
  // {
  //   // paData->index = 0;
  //   return paComplete;
  // }
  else
  {
      for (i = 0; i < framesPerBuffer - 1; i++) {
        // printf("buff: %f \n", paData->buffer[paData->index * paData->sfinfo.channels]);
        *out++ = paData->buffer[paData->index * paData->sfinfo.channels];  // Mono for simplicity
        *out++ = paData->buffer[(paData->index * paData->sfinfo.channels) + 1];  // Mono for simplicity
        paData->index = paData->index + 2;
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
  
  printf("pa_buffer: %f \n", paData->buffer[1008900]);
  printf("pa_buff_frames: %lli \n", paData->buffer_frames);
  printf("pa_idx: %lli \n", paData->index);

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
            FRAMES_PER_BUFFER,
            0, /* paClipOff, */  /* we won't output out of range samples so don't bother clipping them */
            callback,
            paData );
  if( err != paNoError ) goto error;

  err = Pa_StartStream( stream );
  if( err != paNoError ) goto error;

  printf("Hit ENTER to stop program.\n");
  getchar();
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
