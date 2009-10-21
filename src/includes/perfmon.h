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
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License, v2, as
 *      published by the Free Software Foundation
 *     
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *     
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
extern void perfmon_setCycles(uint64_t);

extern void perfmon_markerStartCounters(int core_id);
extern void perfmon_markerStopCounters(int core_id);
extern void perfmon_markerSetCycles(uint64_t cycles);

#endif /*PERFMON_H*/
