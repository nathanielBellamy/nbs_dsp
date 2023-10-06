#ifndef PA_DATA_H
#define PA_DATA_H
#include <sndfile.h>

typedef struct {
    SNDFILE *file;
    SF_INFO sfinfo;
    float *buffer;
    sf_count_t index;
    sf_count_t buffer_frames;
} PA_DATA;

#endif
