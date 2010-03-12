/*
 * ===========================================================================
 *
 *       Filename:  perfmon_types.h
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

#ifndef PERFMON_TYPES_H
#define PERFMON_TYPES_H

#include <bstrlib.h>

/* #####   EXPORTED TYPE DEFINITIONS   #################################### */

typedef enum {
    PMC0 = 0,
    PMC1,
    PMC2,
    PMC3,
    PMCU0,
    PMCU1,
    PMCU2,
    PMCU3,
    PMCU4,
    PMCU5,
    PMCU6,
    PMCU7,
    NUM_PMC} PerfmonCounterIndex;

typedef enum {
    NOGROUP = 0,
    FLOPS_DP,
    FLOPS_SP,
    L1,
    L2,
    L3,
    MEM,
    DATA,
    BRANCH,
    CPI,
    FRONTEND,
    CLUSTER,
    CLUSTER_FLOPS,
    TLB} PerfmonGroup;

typedef struct {
    uint32_t event_id;
    uint32_t umask;
} PerfmonEvent;

typedef struct {
    char*  key;
    PerfmonEvent  event;
} PerfmonHashEntry;

typedef struct {
    bstring  label;
    int  init;
    uint64_t  config_reg;
    uint64_t  counter_reg;
} PerfmonCounter;

typedef struct {
    int cpu_id;
    uint64_t cycles;
    uint64_t instructionsRetired;
    PerfmonCounter counters[NUM_PMC];
    uint64_t pc[NUM_PMC];
} PerfmonThread;

typedef struct {
    bstring reg;
    bstring eventName;
    PerfmonEvent event;
    PerfmonCounterIndex index;
    double* results;
    double* time;
} PerfmonEventSetEntry;

typedef struct {
    int numberOfEvents;
    PerfmonEventSetEntry* events;
} PerfmonEventSet;


#endif /*PERFMON_TYPES_H*/
