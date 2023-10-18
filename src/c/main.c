#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>
#include <stdlib.h>
#include <fftw3.h>
#include "pa.h"
#include "pa_data.h"
#include "visual.h"
#include "../cpp/foo.h"
#include "visual.h"

// pa.h
int init_pa(PA_DATA *paData);
void freePaData(PA_DATA *paData);
void *pa(void *paData);

// visual.h

// ../cpp/foo.h
void bar(void);

int main(void);
int main(void) {
  bar();
  
  // init data
  PA_DATA paData;
  if ( init_pa(&paData) != 0)
  {
    freePaData(&paData);
    return 1;
  };

  // init threads
  pthread_t thread_audio, thread_visual;
  int ta_create_err = pthread_create(
    &thread_audio, 
    NULL, 
    pa, // TODO: make pa the right type to pass to pthread
    &paData
  );
  if (ta_create_err)
  {
    fprintf(stderr, "\nError - audio pthread_create() return code: %d", ta_create_err);
    return 1;
  }

  int tv_create_err = pthread_create(
    &thread_visual,
    NULL,
    visualMain,
    NULL
  );
  if (tv_create_err)
  {
    fprintf(stderr, "\nError - visual pthread_create() return code: %d", ta_create_err);
    return 1;
  }

  // join threads
  int ta_join_err = pthread_join(thread_audio, NULL);
  if ( ta_join_err )
  {
    fprintf(stderr, "\nError - audio pthread_join() return code: %d", ta_join_err);
  }

  int tv_join_err = pthread_join(thread_visual, NULL);
  if ( tv_join_err )
  {
    fprintf(stderr, "\nError - visual pthread_join() return code: %d", tv_join_err);
  }

  // Cleanup
  freePaData(&paData);
  return 0;
}
