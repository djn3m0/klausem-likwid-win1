#ifndef LIBPERFCTR_H
#define LIBPERFCTR_H

#include <bstrlib.h>

typedef struct {
    bstring  tag;
    double*  time;
    double** counters;
} LikwidResults;

#endif /*LIBPERFCTR_H*/
