#ifndef AUDIO_DATA_H
#define AUDIO_DATA_H
#include <stdatomic.h>
#include <sndfile.h>
#include <fftw3.h>

typedef struct {
    // to be accessed by visual thread
    atomic_int *atomicCounter;
    atomic_int *debugInt;
    atomic_int *atomicEQ;
    atomic_int *atomicEqSync;

    // for portaudio only
    SNDFILE *file;
    SF_INFO sfinfo;
    float *buffer;
    sf_count_t index;

  //
  // NOTE:
  // - we cannot allocate/free memory within the pa_callback (see docs - callback may be called at system interrupt level)
  // - so we allocate what memory we need here
    fftwf_plan fft_plan_to_freq; // plan used to compute the FFT, (): time_domain -> freq_domain
    fftwf_plan fft_plan_to_time; // plan used to compute the FFT, (): freq_domain -> time_domain
    float *fft_buffer; // - copy float paData.buffer being transformed this callback
    float *fft_time; // - copy of channel data from paData.buffer being transformed this callback
                     // - the fft_plan_to_freq will overwrite this array on execution
                     //   - we do not rely upon these values after calling this plan
                     //   - but the plan does not de-allocate memory (not allowed in pa_callback)
                     // - the fft_plan_to_time will output into this array
    fftwf_complex *fft_freq; // array storing results of fft

  // paData.buffer --copy-->
  //   paData.fft_buffer --for each channel, copy-->
  //     paData.fft_time --fft_plan_to_freq-->
  //       paData.fft_freq --destructive, in-place, "fx" transformation-->
  //         paData.fft_freq --fft_plan_to_time-->
  //           paData.fft_time --normalize-->
  //             paData.fft_time --copy into indices mod channel count (interwoven)-->
  //              paData.fft_buffer --copy-->
  //                pa_out
} AUDIO_DATA;

#endif
