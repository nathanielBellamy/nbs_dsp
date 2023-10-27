#ifndef VISUAL_DATA_H
#define VISUAL_DATA_H
#include <stdatomic.h>

typedef struct {
    atomic_int *atomicCounter;
    atomic_int *atomicEQ;
    int buffer_frames;
    int buffer_frames_d2p1;
} VISUAL_DATA;

#endif
