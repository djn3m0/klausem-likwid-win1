/*
 * ===========================================================================
 *
 *       Filename:  perfmon.h
 *
 *    Description:  Header File of perfmon module.
 *                  Configures and reads out performance counters
 *                  on x86 based architectures. Supports multi threading.
 *
 *        Version:  1.0
 *        Created:  07/15/2009
 *       Revision:  none
 *
 *         Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *        Company:  RRZE Erlangen
 *        Project:  none
 *      Copyright:  Copyright (c) 2009, Jan Treibig
 *
 * ===========================================================================
 */


/** \file perfmon.h
 * Header of perfmon module
 *
 */
#ifndef PERFMON_H
#define PERFMON_H

#include <types.h>

extern int perfmon_verbose;


extern void perfmon_init (int numThreads, int threads[]);
extern int  perfmon_setupCounter (PerfmonCounterIndex index, char* event);
extern int  perfmon_setupGroup(char* );
extern void perfmon_startAllCounters (void);
extern void perfmon_stopAllCounters (void);
extern void perfmon_startCounters (int thread_id);
extern void perfmon_stopCounters (int thread_id);
extern void (*perfmon_printAvailableGroups)(void);
extern void perfmon_printResults(void);
extern void perfmon_getCycles(void);
extern void perfmon_setSycles(uint64_t);

extern void perfmon_markerStartCounters(int core_id);
extern void perfmon_markerStopCounters(int core_id);
extern void perfmon_markerSetCycles(uint64_t cycles);

static int getEvent(char* event_str, uint32_t* event, uint32_t* umask);
static void initThread(int thread_id, int cpu_id);
static int setupCounterThread(int thread_id,
        PerfmonCounterIndex index,
        char* event_str);
static void setupGroupThread(int thread_id, PerfmonGroup group);
static void startCountersThread(int thread_id);
static void stopCountersThread(int thread_id);


#endif /*PERFMON_H*/
