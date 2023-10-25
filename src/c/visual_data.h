#ifndef VISUAL_DATA_H
#define VISUAL_DATA_H
#include <stdatomic.h>

typedef struct {
    atomic_int *atomicCounter;
    atomic_int *atomicEQ;
    int buffer_frames;
} VISUAL_DATA;

#endif
