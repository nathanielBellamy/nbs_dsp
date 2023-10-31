#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include "visual.h"
#include "visual_data.h"
#include "../cpp/extern_c.h"

// extern_c.h
void drawGraph(float* counter);

void *visualMain(void *visualData_) 
{
  sleep(1); // let audio thread print its init data
  //
  // TODO:
  //  - implement double buffer
  //  - only render change 
  //  - prevent flicker
  int frameRate = 3000000; // 15000000;
  int frameCounter = 0;
  // TODO:
  // float smoothing_f = 1000.0;
  // int smoothing_i = 1000;

  VISUAL_DATA *visualData = (VISUAL_DATA *) visualData_;
  int bufferAtomicEq[100][2 * visualData->buffer_frames_d2p1];
  float bufferAtomicEq_norm[100][2 * visualData->buffer_frames_d2p1];
  float bufferAtomicEq_avg[2 * visualData->buffer_frames_d2p1];
  // bufferAtomicEq = (int *) malloc(2 * visualData->buffer_frames_d2p1 * sizeof(int));
  // bufferAtomicEq_norm = (float *) malloc(2 * visualData->buffer_frames_d2p1 * sizeof(float));

  while( true )
  {
    frameCounter += 1;
    if (frameCounter == frameRate - 100)
    {
      system("clear");
    } 
    else if (frameCounter >= frameRate - 100 && frameCounter < frameRate) 
    {
      // int val = atomic_load(visualData->atomicCounter);
      // printf("\n%d\n", val);
        //
      int frameIndex = frameRate - frameCounter;

      // TODO:
      //   - maxMag should be per channel
      int maxMag = 1;
      for (int ch = 0; ch < 2; ch++)
      {
        for (int i = 0; i < visualData->buffer_frames_d2p1; i++)
        {
          int index = i + (ch * visualData->buffer_frames_d2p1);
          bufferAtomicEq[frameIndex][index] = atomic_load(visualData->atomicEQ + index); // load ith atomic EQ
          if ( bufferAtomicEq[frameIndex][index] > maxMag  && i < 18)
          {
            maxMag = bufferAtomicEq[frameIndex][index];
          }
        }
      }

      for (int i = 0; i < 2 * visualData->buffer_frames_d2p1; i++)
      {
        // bufferAtomicEq_norm[i] = ( bufferAtomicEq[i] ) / / / );
        bufferAtomicEq_norm[frameIndex][i] = (float) sqrtf( bufferAtomicEq[frameIndex][i] / 100.0 );
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
        for (int j = 0; j < 100; j++)
        {
          total += bufferAtomicEq_norm[j][i];
        }
        bufferAtomicEq_avg[i] = total / 100.0;
      }
      drawGraph(bufferAtomicEq_avg);
      frameCounter = 0;
    }
  }

  // free(bufferAtomicEq);
  // free(bufferAtomicEq_norm);
  printf("\nVisual Thread Signing Off");
  pthread_exit((void *) 0);
}
