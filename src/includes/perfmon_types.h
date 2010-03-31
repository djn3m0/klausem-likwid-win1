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
    PMC4,
    PMC5,
    PMC6,
    PMC7,
    PMC8,
    PMC9,
    PMC10,
    PMC11,
    PMC12,
    PMC13,
    NUM_PMC} PerfmonCounterIndex;

typedef enum {
    NOGROUP = 0,
    FLOPS_DP,
    FLOPS_SP,
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
    char* key;
    PerfmonCounterIndex index;
} PerfmonCounterMap;

typedef struct {
    char* key;
    PerfmonGroup index;
    char* info;
} PerfmonGroupMap;



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
    PerfmonType  type;
    int  id;
    uint64_t  config_reg;
    uint64_t  counter_reg;
} PerfmonCounter;

typedef struct {
    int cpu_id;
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


typedef struct {
    bstring label;
    double* value;
} PerfmonResult;

typedef struct {
    bstrList* header;
    int numRows;
    int numColumns;
    PerfmonResult* rows;
} PerfmonResultTable;


#endif /*PERFMON_TYPES_H*/
