/*
 * ===========================================================================
 *
 *       Filename:  perfmon_k8.h
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

#include <stdlib.h>
#include <stdio.h>

#include <bstrlib.h>
#include <types.h>
#include <registers.h>
#include <perfmon_k8_events.h>

#define NUM_GROUPS_K8 8

static int perfmon_numGroupsK8 = NUM_GROUPS_K8;
static int perfmon_numArchEventsK8 = NUM_ARCH_EVENTS_K8;

static PerfmonGroupMap k8_group_map[NUM_GROUPS_K8] = {
    {"FLOPS_DP",FLOPS_DP,"Double Precision MFlops/s","SSE_RETIRED_ADD_DOUBLE_FLOPS:PMC0,SSE_RETIRED_MULT_DOUBLE_FLOPS:PMC1,CPU_CLOCKS_UNHALTED:PMC2"},
    {"FLOPS_SP",FLOPS_SP,"Single Precision MFlops/s","SSE_RETIRED_ADD_SINLGE_FLOPS:PMC0,SSE_RETIRED_MULT_SINLGE_FLOPS:PMC1,CPU_CLOCKS_UNHALTED:PMC2"},
    {"L2",L2,"L2: L2 cache bandwidth in MBytes/s","DATA_CACHE_REFILLS_L2_ALL:PMC0,DATA_CACHE_EVICTED_ALL:PMC1,CPU_CLOCKS_UNHALTED:PMC2"},
    {"MEM",MEM,"Main memory bandwidth in MBytes/s",""},
    {"DATA",DATA,"Load to store ratio","INST_RETIRED_LOADS:PMC0,INST_RETIRED_STORES:PMC1,CPU_CLOCKS_UNHALTED:PMC2"},
    {"BRANCH",BRANCH,"Branch prediction miss rate","BRANCH_RETIRED:PMC0,BRANCH_MISPREDICT_RETIRED:PMC1,CPU_CLOCKS_UNHALTED:PMC2"},
    {"CPI",CPI,"CPI: cycles per instruction","UOPS_RETIRED:PMC0,CPU_CLOCKS_UNHALTED:PMC1"},
    {"TLB",TLB,"Translation lookaside buffer miss rate","DTLB_L2_MISS_4K:PMC0,CPU_CLOCKS_UNHALTED:PMC1"}
};


