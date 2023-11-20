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
void drawHeader(void* visualData);
void updateGraph(
  double (*polynomialArray)[16][16],
  char (*graphCurr)[32][64],
  char (*graphNext)[32][64],
  int offsetX,
  int offsetY,
  void *settingsIn
);
int xStepCount(void* settings);
double stepWidth(void* settings);
double stepHeight(void* settings);

void *visualMain(void *visualData_) 
{
  int frameRate = 15000000; // 15000000;
  int frameCounter = 0;

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
  settings.displayWidth = 64;
  settings.displayHeight = 32;
  settings.stepWidth = stepWidth((void *) &settings);
  settings.stepHeight = stepHeight((void *) &settings);
  settings.xStepCount = xStepCount((void *) &settings);

  char graphCurrL[32][64];
  char graphCurrR[32][64];

  char graphNextL[32][64];
  char graphNextR[32][64];
  
  double polynomialArrayL[16][16];
  double polynomialArrayR[16][16];

  //
  // RENDER
  // 
  system("clear");

  drawHeader((void*) &visualData);


  // TODO:
  //   -[X] L + R graphs
  //   -[ ] header
  //   -[ ] footer
  //   -[ ] border
  //   -[ ] pick a band
  //   -[ ] increase/decrease boost with up/down keys

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
        bufferAtomicEq_norm[frameIndex][i] = (float) 64.0 * sqrtf( bufferAtomicEq[frameIndex][i] ) / maxMag;
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

      for (int i = 0; i < 16; i++)
      {
        polynomialArrayL[i][0] = (double) (bufferAtomicEq_avg[i + 1]);
        for (int j = 1; j < 16; j++)
        {
          polynomialArrayL[i][j] = 0;
        }
      }

      for (int i = 0; i < 16; i++)
      {
        polynomialArrayR[i][0] = (double) (bufferAtomicEq_avg[i + 17]);
        for (int j = 1; j < 16; j++)
        {
          polynomialArrayR[i][j] = 0;
        }
      }
      
      updateGraph(
        &polynomialArrayL,
        &graphCurrL,
        &graphNextL,
        10,
        7,
        (void *) &settings
      );
      
      updateGraph(
        &polynomialArrayR,
        &graphCurrR,
        &graphNextR,
        85,
        7,
        (void *) &settings
      );

      frameCounter = 0;
    }
  }

  printf("\nVisual Thread Signing Off");
  pthread_exit((void *) 0);
}
