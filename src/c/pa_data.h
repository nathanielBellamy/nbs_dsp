#ifndef PA_DATA_H
#define PA_DATA_H
#include <sndfile.h>
#include <fftw3.h>

// TODO: 
//  fftw_plan fftwPlan
//  float *fftwOutBuffer
typedef struct {
    SNDFILE *file;
    SF_INFO sfinfo;
    float *buffer;
    sf_count_t index;
    sf_count_t buffer_frames; // frames processed per callback invocation
  //
  //
  // NOTE: 
  // - we cannot allocate/free memory within the pa_callback (see docs - callback may be called at system interrupt level)
  // - so we allocate what memory we need here
    fftw_plan fft_plan_to_freq; // plan used to compute the FFT, (): time_domain -> freq_domain
    fftw_plan fft_plan_to_time; // plan used to compute the FFT, (): freq_domain -> time_domain
    fftw_complex *fft_out; // array storing results of fft
                           // to be read into pa out buffer while
                           //   - mapping back to time domain space
                           //   - normalizing to unit values
                           // .wav_buffer --fft_plan_to_freq--> 
                           //   paData.fft_out --destructive, in-place, "fx" transformation--> 
                           //     paData.fft_out --fft_plan_to_time--> pa_out
} PA_DATA;

#endif
