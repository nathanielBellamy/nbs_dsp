#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include "settings.h"
#include "visual.h"
#include "visual_data.h"
#include "../cpp/extern_c.h"

// extern_c.h
// void drawGraph(float* bufferAtomicEq_avg, void *settingsIn);
void updateGraph(
  float* bufferAtomicEq_avg,
  char (*graphCurr)[30][80],
  char (*graphNext)[30][80],
  int offsetX,
  int offsetY,
  void *settingsIn
);
int xStepCount(void* settings);
double stepWidth(void* settings);
double stepHeight(void* settings);

void *visualMain(void *visualData_) 
{
  // TODO:
  //  - the visual thread should display the data
  sleep(1); // let audio thread print its init data

  int frameRate = 5000000; // 15000000;
  int frameCounter = 0;
  // TODO:
  float smoothing_f = 1500.0;
  int smoothing_i = 1500;

  VISUAL_DATA *visualData = (VISUAL_DATA *) visualData_;
  int bufferAtomicEq[smoothing_i][2 * visualData->buffer_frames_d2p1];
  float bufferAtomicEq_norm[smoothing_i][2 * visualData->buffer_frames_d2p1];
  float bufferAtomicEq_avg[2 * visualData->buffer_frames_d2p1];

  // TODO:
  // - get and update settings from user
  SETTINGS settings;
	settings.backgroundChar= '.';
	settings.originChar = '+';
	settings.xAxisChar = '-';
	settings.yAxisChar = '|';
	settings.xMin = -1.3;
	settings.xMax = 1.3;
	settings.yMin = 0.0;
	settings.yMax = 1.0;
	settings.epsilon = 0.05;
  settings.displayWidth = 80;
  settings.displayHeight = 30;
  settings.stepWidth = stepWidth((void *) &settings);
  settings.stepHeight = stepHeight((void *) &settings);
  settings.xStepCount = xStepCount((void *) &settings);

  char graphCurr[30][80];
  char graphNext[30][80];

  //
  // RENDER
  // 
  system("clear");

  printf("\n======>>NBS<<==>>DSP<<========");

  // Display some information about the file.
  printf("\nSample rate: %d", visualData->audioData->sfinfo.samplerate);
  printf("\nChannels: %d", visualData->audioData->sfinfo.channels);
  printf("\nFrames: %lli", visualData->audioData->sfinfo.frames);
  printf("\nFormat: %dl", visualData->audioData->sfinfo.format);

  // TODO:
  //   - L + R graphs
  //   - header
  //   - footer
  //   - border
  //   - pick a band
  //   - increase/decrease boost with up/down keys

  while( true )
  {
    frameCounter += 1;
    if ( frameCounter == frameRate - smoothing_i )
    {
      // system("clear");
    } 
    else if (frameCounter >= frameRate - smoothing_i && frameCounter < frameRate) 
    {
      // int val = atomic_load(visualData->atomicCounter);
      // printf("\n%d\n", val);
      int frameIndex = frameRate - frameCounter;

      // TODO:
      //   - maxMag should be per channel
      int maxPower = 1;
      for (int ch = 0; ch < 2; ch++)
      {
        for (int i = 0; i < visualData->buffer_frames_d2p1; i++)
        {
          int index = i + (ch * visualData->buffer_frames_d2p1);
          bufferAtomicEq[frameIndex][index] = atomic_load(visualData->atomicEQ + index); // load ith atomic EQ
          if ( bufferAtomicEq[frameIndex][index] > maxPower  && i < 18)
          {
            maxPower = bufferAtomicEq[frameIndex][index];
          }
        }
      }
      float maxMag = sqrt( (float) maxPower );

      for (int i = 0; i < 2 * visualData->buffer_frames_d2p1; i++)
      {
        bufferAtomicEq_norm[frameIndex][i] = (float) 80.0 * sqrtf( bufferAtomicEq[frameIndex][i] ) / maxMag;
      }
    }
    else if ( frameCounter == frameRate ) 
    {
      for (int i = 0; i < 2 * visualData->buffer_frames_d2p1; i++)
      {
        float total = 0.0;
        for (int j = 0; j < smoothing_i; j++)
        {
          total += bufferAtomicEq_norm[j][i];
        }
        bufferAtomicEq_avg[i] = total / smoothing_f;
      }
      
      updateGraph(
        bufferAtomicEq_avg,
        &graphCurr,
        &graphNext,
        5,
        5,
        (void *) &settings
      );

      frameCounter = 0;
    }
  }

  printf("\nVisual Thread Signing Off");
  pthread_exit((void *) 0);
}
