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
  VISUAL_DATA *visualData = (VISUAL_DATA *) visualData_;
  while( true )
  {
    frameCounter += 1;
    int val = atomic_load(visualData->atomicCounter);
    if (frameCounter == frameRate - 1)
    {
      system("clear");
    } 
    else if (frameCounter == frameRate) {
      printf("\n%d\n", val);
      drawGraph(val);
      frameCounter = 0;
    }
  }
  printf("\nVisual Thread Signing Off");
  pthread_exit((void *) 0);
}
