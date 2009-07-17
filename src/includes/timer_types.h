#ifndef TIMER_TYPES_H
#define TIMER_TYPES_H

typedef union
{  
    uint64_t int64;
    struct {uint32_t lo, hi;} int32;
} TscCounter;

#endif /*TIMER_TYPES_H*/
