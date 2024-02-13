#ifndef VISUAL_DATA_H
#define VISUAL_DATA_H
#include <stdatomic.h>
#include "audio_data.h"

typedef struct {
    atomic_int *atomicCounter;
    atomic_int *debugInt;
    atomic_int *atomicEQ;
    atomic_int *atomicEqSync;
    AUDIO_DATA *audioData;
} VISUAL_DATA;

#endif
