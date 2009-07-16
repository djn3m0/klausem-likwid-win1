#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>
#include <types.h>

typedef struct time {
    struct timeval before;
    struct timeval after;
} TimerData;

typedef struct cycles {
    uint64_t start;
    uint64_t stop;
    uint64_t over;
} CyclesData;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  initTimer
 *  Description:  Initializes cycles for cpuid and cpuclock
 * =====================================================================================
 */
extern void initTimer(void );


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  startTimer
 *  Description:  Store timestamp in TimerData->before with gettimeofday()
 * =====================================================================================
 */
extern void starttimer(TimerData* );


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  stopTimer
 *  Description:  Store timestamp in TimerData->after with gettimeofday()
 * =====================================================================================
 */
extern void stoptimer(TimerData* );


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  printTimer
 *  Description:  Return time duration between TimerData->after - TimerData->before .
 * =====================================================================================
 */
extern float printtimer(TimerData* );


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  readCycles
 *  Description:  Gets cycle counter with rdtsc instruction.
 *                Serialized with cpuid.
 *                Defined in tsc-asm.pas .
 *                Overhead must be determined by user.
 * =====================================================================================
 */
extern void readCycles(uint64_t* );


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  cyclesForCpuId
 *  Description:  Returns cycles for cpuid.
 *                Defined in tsc-asm.pas.
 * =====================================================================================
 */
extern int cyclesForCpuId(void);


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  cpuClockFrequency
 *  Description:  Determines cpu clock frequency from /proc filesystem
 * =====================================================================================
 */
extern float cpuClockFrequency(void);

void startCycles(CyclesData* time);
float stopCycles(CyclesData* time);

/* defined in timer.c */
extern int baseCycles;
extern float cpuclock;

#endif /* TIMER_H */
