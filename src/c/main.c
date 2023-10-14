#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include "pa.h"
#include "pa_data.h"
#include "../cpp/foo.h"

// pa.h
int pa(PA_DATA *paData);

// ../cpp/foo.h
void bar(void);

int main(void);
int main(void) {
  bar();
  
  PA_DATA paData;
  paData.index = 0;
  paData.buffer_frames = 32;
  printf("padidx: %lli", paData.index);

  if (! (paData.file = sf_open("gtfam.wav", SFM_READ, &paData.sfinfo)))
  {
		printf ("Not able to open input file.\n") ;
		/* Print the error message from libsndfile. */
		puts (sf_strerror (NULL)) ;
    return 1 ;
  };
  
  // Display some information about the file.
  printf("Sample rate: %d \n", paData.sfinfo.samplerate);
  printf("Channels: %d \n", paData.sfinfo.channels);
  printf("Frames: %lli \n", paData.sfinfo.frames);
  
  // Allocate memory for data
  paData.buffer = (float *) malloc(paData.sfinfo.frames * paData.sfinfo.channels * sizeof(float));
  if (!paData.buffer) {
      printf("Cannot allocate memory\n");
      return 1;
  }

  // Read the audio data into buffer
  long readcount = sf_read_float(paData.file, paData.buffer, paData.sfinfo.frames * paData.sfinfo.channels);
  
  printf("readcount: %ld\n", readcount);

  // TODO:
  //
  paData.fft_time = (float*) fftw_malloc(sizeof(float) * paData.buffer_frames);
  paData.fft_freq = (fftwf_complex*) fftw_malloc(sizeof(fftwf_complex) * paData.buffer_frames);
  paData.fft_plan_to_freq = fftwf_plan_dft_r2c_1d(
    paData.buffer_frames, 
    paData.fft_time,
    paData.fft_freq, 
    FFTW_ESTIMATE
  );
  // paData.fft_plan_to_time = 

  if ( pa(&paData) != 0 )
  {
    free(paData.buffer);
    free(paData.fft_freq);
    free(paData.fft_time);
    fftwf_destroy_plan(paData.fft_plan_to_freq);
    fftwf_destroy_plan(paData.fft_plan_to_time);
    sf_close(paData.file);
    return 1;
  }

  // Cleanup
  printf("\nCleaning up resources...");
  free(paData.buffer);
  free(paData.fft_freq);
  free(paData.fft_time);
  fftwf_destroy_plan(paData.fft_plan_to_freq);
  fftwf_destroy_plan(paData.fft_plan_to_time);
  sf_close(paData.file);

  printf("\nDone.");
  return 0;
}
