#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>
#include <stdlib.h>
#include <fftw3.h>
#include "pa.h"
#include "pa_data.h"
#include "../cpp/foo.h"

// pa.h
int pa(PA_DATA *paData);
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
  
  PA_DATA paData;
  init_pa(&paData);

  if ( pa(&paData) != 0 )
  {
    // Log error
    printf("\nportaudio encountered an error.");
    
    // Cleanup
    freePaData(&paData);
    return 1;
  }

  // Cleanup
  freePaData(&paData);
  return 0;
}
