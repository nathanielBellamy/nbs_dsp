#ifndef VISUAL_DATA_H
#define VISUAL_DATA_H
#include <stdatomic.h>
#include "audio_data.h"

typedef struct {
    atomic_int *atomicCounter;
    atomic_int *debugInt;
    atomic_int *atomicEQ;
    int buffer_frames;
    int buffer_frames_d2p1;
    AUDIO_DATA *audioData;
} VISUAL_DATA;

#endif
