#ifndef LIBPERFCTR_H
#define LIBPERFCTR_H

#include <bstrlib.h>

typedef struct {
    bstring  tag;
    double*  cycles;
    double*  instructions;
    double*  time;
    double** counters;
} LikwidResults;

#endif /*LIBPERFCTR_H*/
