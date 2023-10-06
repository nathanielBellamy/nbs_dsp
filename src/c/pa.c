#include <stdio.h>
#include <portaudio.h>
#include "pa.h"
#include "pa_data.h"

// following: https://github.com/PortAudio/portaudio/blob/master/examples/pa_fuzz.c

#define PA_SAMPLE_TYPE      paFloat32
#define FRAMES_PER_BUFFER   (64)

typedef float SAMPLE;

static int bufferIdx = 0;

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
  const SAMPLE *fileBuffer = userData;

  int iMax = bufferIdx + framesPerBuffer - 1;

  if( fileBuffer == NULL )
  {
      for( i=bufferIdx; i<iMax; i++ )
      {
          *out++ = 0;  /* left - silent */
          *out++ = 0;  /* right - silent */
      }
  }
  else 
  {
      for( i=bufferIdx; i<iMax; i++ )
      {
          SAMPLE sampleLeft = fileBuffer[i];
          *out++ = sampleLeft; // place left
          SAMPLE sampleRight = fileBuffer[i+1];
          *out++ = sampleRight; // place right
      }
  }

  bufferIdx += 2;
  return paContinue;
}

int pa(PA_DATA *paData)
{
  PaStreamParameters inputParameters, outputParameters;
  PaStream *stream;
  PaError err;

  err = Pa_Initialize();
  if( err != paNoError ) goto error;

  inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
  if (inputParameters.device == paNoDevice) {
      fprintf(stderr,"Error: No default input device.\n");
      goto error;
  }
  inputParameters.channelCount = 2;       /* stereo in from file */
  inputParameters.sampleFormat = PA_SAMPLE_TYPE;
  inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
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
            callback, // TODO
            paData->buffer );
  if( err != paNoError ) goto error;

  err = Pa_StartStream( stream );
  if( err != paNoError ) goto error;

  printf("Hit ENTER to stop program.\n");
  getchar();
  err = Pa_CloseStream( stream );
  if( err != paNoError ) goto error;

  Pa_Terminate();
  return 0;

  error:
    Pa_Terminate();
    fprintf( stderr, "An error occurred while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return -1;
}
