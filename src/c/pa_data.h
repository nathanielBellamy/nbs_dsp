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
    fftwf_plan fft_plan_to_freq; // plan used to compute the FFT, (): time_domain -> freq_domain
    fftwf_plan fft_plan_to_time; // plan used to compute the FFT, (): freq_domain -> time_domain
    float *fft_time; // - copy float paData.buffer being transformed this callback
                   // - the fft_plan_to_freq will overwrite this array on execution
                   //   - we do not rely upon these values after calling this plan
                   //   - but the plan does not de-allocate memory (not allowed in pa_callback)  
                   // - the fft_plan_to_time will output into this array
                   // - the data is then read from here into the pa_out_buffer
    fftwf_complex *fft_freq; // array storing results of fft
                            // to be read into pa out buffer while
                            //   - mapping back to time domain
                            //   - normalizing to unit values - factor of paData.sf_info.buffer_frames
                            // paData.buffer --copy--> 
                            //   paData.fft_time --fft_plan_to_freq--> 
                            //     paData.fft_freq --destructive, in-place, "fx" transformation-->
                            //       paData.fft_freq --fft_plan_to_time-->
                            //         paData.fft_time --normalize--> 
                            //           paData.fft_time --copy--> 
                            //             pa_out
} PA_DATA;

#endif
