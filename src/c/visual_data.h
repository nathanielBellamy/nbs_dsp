#ifndef VISUAL_DATA_H
#define VISUAL_DATA_H
#include <stdatomic.h>

typedef struct {
    // to be accessed by visual thread
    atomic_int *atomicCounter;
} VISUAL_DATA;

#endif
