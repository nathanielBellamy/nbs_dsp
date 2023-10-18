#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include "visual.h"

void *visualMain(void *foo) 
{
  printf("\nVisual Thread Started");
  sleep(5);
  printf("\nVisual Thread slept a bit.");
  sleep(10);
  printf("\nVisual Thread slept a bit more.");
  sleep(1);
  printf("\nVisual Thread Signing Off");
  pthread_exit((void *) 0);
}
