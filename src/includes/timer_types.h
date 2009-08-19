/**
 *       @file  timer_types.h
 *      @brief  Types of timer module
 *
 * Detailed description starts here.
 *
 *     @author  Jan Treibig (jt), jan.treibig@gmail.com
 *
 *   @internal
 *     Created  07/30/2009
 *    Revision  ---
 *    Compiler  gcc
 *     Company  RRZE Erlangen
 *   Copyright  Copyright (c) 2009, Jan Treibig
 *
 * ============================================================================
 */
#ifndef TIMER_TYPES_H
#define TIMER_TYPES_H

#include <stdint.h>
#include <sys/time.h>


/**
 * @brief Union for cycles measurements
 * 
 * Union with either one 64 bit unsigned integer or a struct
 * of two 32 bit unsigned integers used as register eax and ebx
 * in call to rdtsc.
 */
typedef union
{  
    uint64_t int64;                   /** 64 bit unsigned integer fo cycles */
    struct {uint32_t lo, hi;} int32;  /** two 32 bit unsigned integers used
                                        for register values */
} TscCounter;

/**
 * @brief Struct holding the start and stop timestamp
 * 
 * A reference to this struct is passed to the timer functions and hold the
 * timestamps.
 */
typedef struct {
    struct timeval before;
    struct timeval after;
} TimerData;

/**
 * @brief Struct holding the start, stop and base timestamps
 * 
 * A reference to this struct is passed to the cycles functions and holds
 * the start and stop timestamp. Additionally base holds a possible overhead
 * for empty measurement or cpuid serialization.
 */

typedef struct {
    TscCounter start;
    TscCounter stop;
    uint64_t base;
} CyclesData;


#endif /*TIMER_TYPES_H*/
