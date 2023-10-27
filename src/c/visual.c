#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>
#include <unistd.h>
#include "visual.h"
#include "visual_data.h"
#include "../cpp/extern_c.h"

// extern_c.h
void drawGraph(int counter);

void *visualMain(void *visualData_) 
{
  int frameRate = 15000000;
  int frameCounter = 0;
  int *bufferAtomicEQ;

  VISUAL_DATA *visualData = (VISUAL_DATA *) visualData_;
  bufferAtomicEQ = (int *) malloc(visualData->buffer_frames * sizeof(int));

  while( true )
  {
    frameCounter += 1;
    if (frameCounter == frameRate - 1)
    {
      system("clear");
    } 
    else if (frameCounter == frameRate) 
    {
      int val = atomic_load(visualData->atomicCounter);
      printf("\n%d\n", val);

      for (int ch = 0; ch < 2; ch++)
      {
        printf("\n Channel \n");
        for (int i = 0; i < visualData->buffer_frames_d2p1; i++)
        {
          int index = i + (ch * visualData->buffer_frames_d2p1);
          bufferAtomicEQ[index] = atomic_load(visualData->atomicEQ + index); // load ith atomic EQ
          if (i < 3)
          {
            printf(" ==>> %i, %i <<==\n", i, bufferAtomicEQ[i]);
          }
        }
        printf("\n");
      }



      // TODO:
        //  drawGraph(val, &atomicEQ)
        //  
        //  - atomicEQ[0-31] is 32 band EQ for left
        //  - atomicEQ[32-63] is 32 band EQ for right

      drawGraph(val);
      frameCounter = 0;
    }
  }

  free(bufferAtomicEQ);
  printf("\nVisual Thread Signing Off");
  pthread_exit((void *) 0);
}
