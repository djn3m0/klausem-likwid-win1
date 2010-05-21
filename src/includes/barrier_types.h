#ifndef BARRIER_TYPES_H
#define BARRIER_TYPES_H

#include <stdint.h>

typedef struct {
    int        numberOfThreads;
    int        offset;
    int        val;
    int*       index;
    volatile int*  bval;
} BarrierData;

typedef struct {
    int*       groupBval;
    int        numberOfThreads;
} BarrierGroup;

#endif /*BARRIER_TYPES_H*/
