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
  int frameRate = 15000000;
  int frameCounter = 0;
  int *bufferAtomicEQ;
  float *bufferAtomicEQ_norm;

  VISUAL_DATA *visualData = (VISUAL_DATA *) visualData_;
  bufferAtomicEQ = (int *) malloc(2 * visualData->buffer_frames_d2p1 * sizeof(int));
  bufferAtomicEQ_norm = (float *) malloc(2 * visualData->buffer_frames_d2p1 * sizeof(float));

  while( true )
  {
    frameCounter += 1;
    if (frameCounter == frameRate - 5000)
    {
      system("clear");
    } 
    else if (frameCounter == frameRate) 
    {
      // int val = atomic_load(visualData->atomicCounter);
      // printf("\n%d\n", val);

      int maxMag = 1;
      for (int ch = 0; ch < 2; ch++)
      {
        for (int i = 0; i < visualData->buffer_frames_d2p1; i++)
        {
          int index = i + (ch * visualData->buffer_frames_d2p1);
          bufferAtomicEQ[index] = atomic_load(visualData->atomicEQ + index); // load ith atomic EQ
          if ( bufferAtomicEQ[index] > maxMag )
          {
            maxMag = bufferAtomicEQ[index];
          }
        }
      }

      for (int i = 0; i < 2 * visualData->buffer_frames_d2p1; i++)
      {
        bufferAtomicEQ_norm[i] = ( 10.0 * bufferAtomicEQ[i] ) / maxMag;
        // printf(
        //   " ==>> %i, %f <<== \n", 
        //   i, 
        //   bufferAtomicEQ_norm[i] // ( bufferAtomicEQ[i] * 1000.0 ) / ( visualData->buffer_frames * maxMag )
        // );
      }

      system("clear");
      drawGraph(bufferAtomicEQ_norm);

      frameCounter = 0;
    }
  }

  free(bufferAtomicEQ);
  free(bufferAtomicEQ_norm);
  printf("\nVisual Thread Signing Off");
  pthread_exit((void *) 0);
}
