#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>
#include <unistd.h>
#include "visual.h"
#include "visual_data.h"

void *visualMain(void *visualData_) 
{
  VISUAL_DATA *visualData = (VISUAL_DATA *) visualData_;
  while( true )
  {
    printf("\n%d", atomic_load(visualData->atomicCounter));
  }
  printf("\nVisual Thread Signing Off");
  pthread_exit((void *) 0);
}
