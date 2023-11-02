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
void drawGraph(float* bufferAtomicEq_avg, void *settingsIn);
int xStepCount(void* settings);
double stepWidth(void* settings);
double stepHeight(void* settings);

void *visualMain(void *visualData_) 
{
  // TODO:
  //  - the visual thread should display the data
  sleep(1); // let audio thread print its init data

  int frameRate = 3000000; // 15000000;
  int frameCounter = 0;
  // TODO:
  float smoothing_f = 500.0;
  int smoothing_i = 500;

  VISUAL_DATA *visualData = (VISUAL_DATA *) visualData_;
  int bufferAtomicEq[smoothing_i][2 * visualData->buffer_frames_d2p1];
  float bufferAtomicEq_norm[smoothing_i][2 * visualData->buffer_frames_d2p1];
  float bufferAtomicEq_avg[2 * visualData->buffer_frames_d2p1];
  // bufferAtomicEq = (int *) malloc(2 * visualData->buffer_frames_d2p1 * sizeof(int));
  // bufferAtomicEq_norm = (float *) malloc(2 * visualData->buffer_frames_d2p1 * sizeof(float));

  // TODO:
  // -[x]init Settings here 
  // -[ ] init graphCurr[displayHeight][displayWidth]
  // -[ ] init graphNext[displayHeight][displayWidth]
  // -[ ] pass &graphNext to drawGraph and updates 
  // -[ ] loop in here to compare 
  //  -[ ] update only those indices changed in the rendered graph
  // -[ ] set graphCurr = graphNext
  //
  //
  // TODO:
  // - get and update settings from user
  
  SETTINGS settings;
	settings.backgroundChar= '.';
	settings.originChar = '+';
	settings.xAxisChar = '-';
	settings.yAxisChar = '|';
	settings.xMin = -1.3;
	settings.xMax = 1.3;
	settings.yMin = -1.3;
	settings.yMax = 1.3;
	settings.epsilon = 0.1;
  settings.displayWidth = 80;
  settings.displayHeight = 30;
  settings.stepWidth = stepWidth((void *) &settings);
  settings.stepHeight = stepHeight((void *) &settings);
  settings.xStepCount = xStepCount((void *) &settings);





  while( true )
  {
    frameCounter += 1;
    if ( frameCounter == frameRate - smoothing_i )
    {
      system("clear");
    } 
    else if (frameCounter >= frameRate - smoothing_i && frameCounter < frameRate) 
    {
      // int val = atomic_load(visualData->atomicCounter);
      // printf("\n%d\n", val);
        //
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
        // bufferAtomicEq_norm[i] = ( bufferAtomicEq[i] ) / / / );
        bufferAtomicEq_norm[frameIndex][i] = (float) 100.0 * sqrtf( bufferAtomicEq[frameIndex][i] ) / maxMag;
        // printf(
        //   " ==>> %i, %i, %f <<== \n", 
        //   i, 
        //   bufferAtomicEq[i],
        //   bufferAtomicEq_norm[i]
        // );
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
      drawGraph(bufferAtomicEq_avg, (void *) &settings);
      frameCounter = 0;
    }
  }

  printf("\nVisual Thread Signing Off");
  pthread_exit((void *) 0);
}
