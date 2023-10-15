#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <fftw3.h>
#include "pa.h"
#include "pa_data.h"
#include "../cpp/foo.h"

// pa.h
int pa(PA_DATA *paData);

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
  paData.index = 0;
  paData.buffer_frames = 32;
  printf("\npa_idx_start: %lli", paData.index);

  if (! (paData.file = sf_open("gtfam_mini.wav", SFM_READ, &paData.sfinfo)))
  {
		printf ("Not able to open input file.\n") ;
		/* Print the error message from libsndfile. */
		puts (sf_strerror (NULL)) ;
    return 1 ;
  };
  
  // Display some information about the file.
  printf("\nSample rate: %d", paData.sfinfo.samplerate);
  printf("\nChannels: %d", paData.sfinfo.channels);
  printf("\nFrames: %lli", paData.sfinfo.frames);
  
  // Allocate memory for data
  paData.buffer = (float *) malloc(paData.sfinfo.frames * paData.sfinfo.channels * sizeof(float));
  if (!paData.buffer) {
      printf("\nCannot allocate memory");
      return 1;
  }

  // Read the audio data into buffer
  long readcount = sf_read_float(paData.file, paData.buffer, paData.sfinfo.frames * paData.sfinfo.channels);
  
  printf("\nreadcount: %ld", readcount);

  // allocate memory to compute fast fourier transform in pa_callback
  paData.fft_buffer = (float*) fftwf_malloc(sizeof(float) * paData.buffer_frames * paData.sfinfo.channels);
  paData.fft_time = (float*) fftwf_malloc(sizeof(float) * paData.buffer_frames);
  paData.fft_freq = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * paData.buffer_frames);
  paData.fft_plan_to_freq = fftwf_plan_dft_r2c_1d(
    paData.buffer_frames, 
    paData.fft_time,
    paData.fft_freq, 
    FFTW_ESTIMATE
  );
  paData.fft_plan_to_time = fftwf_plan_dft_c2r_1d(
    paData.buffer_frames, 
    paData.fft_freq, 
    paData.fft_time,
    FFTW_ESTIMATE
  );

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
