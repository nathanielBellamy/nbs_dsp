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
    sf_count_t index; // 
    sf_count_t buffer_frames; // frames processed per callback invocation
    fftw_plan fft_plan; // plan used to compute the FFT
    fftw_complex *fft_out; //
} PA_DATA;

#endif
