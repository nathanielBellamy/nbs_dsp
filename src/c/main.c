#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>
#include <stdlib.h>
#include <fftw3.h>
#include "pa.h"
#include "pa_data.h"
#include "../cpp/foo.h"

// pa.h
void *pa(void *paData);
int init_pa(PA_DATA *paData);

void freePaData(PA_DATA *paData);
void freePaData(PA_DATA *paData) {
  printf("\nCleaning up resources...");
  
  free(paData->buffer);
  fftwf_free(paData->fft_buffer);
  fftwf_free(paData->fft_time);
  fftwf_free(paData->fft_freq);
  fftwf_destroy_plan(paData->fft_plan_to_freq);
  fftwf_destroy_plan(paData->fft_plan_to_time);
  sf_close(paData->file);
  
  printf("\nDone.");
};

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
    fprintf(stderr, "\nError - pthread_create() return code: %d", ta_create_err);
    return 1;
  }

  int ta_join_err = pthread_join(thread_audio, NULL);
  if ( ta_join_err )
  {
    fprintf(stderr, "\nError - pthread_join() return code: %d", ta_join_err);
  }

  // Cleanup
  freePaData(&paData);
  return 0;
}
