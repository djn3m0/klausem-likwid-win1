/*
 * ===========================================================================
 *
 *       Filename:  perfmon_k10.h
 *
 *    Description:  AMD K10 specific subroutines
 *
 *        Version:  1.0
 *        Created:  07/15/2009
 *       Revision:  none
 *
 *         Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *        Company:  RRZE Erlangen
 *        Project:  none
 *      Copyright:  Copyright (c) 2010, Jan Treibig
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
#include <perfmon_k10_events.h>

#define NUM_COUNTERS_K10 4
#define NUM_GROUPS_K10 7
#define NUM_SETS_K10 8

static int perfmon_numCountersK10 = NUM_COUNTERS_K10;
static int perfmon_numGroupsK10 = NUM_GROUPS_K10;
static int perfmon_numArchEventsK10 = NUM_ARCH_EVENTS_K10;

static PerfmonCounterMap k10_counter_map[NUM_COUNTERS_K10] = {
    {"PMC0",PMC0},
    {"PMC1",PMC1},
    {"PMC2",PMC2},
    {"PMC3",PMC3}
};

static PerfmonGroupMap k10_group_map[NUM_GROUPS_K10] = {
    {"FLOPS_DP",FLOPS_DP,"Double Precision MFlops/s","SSE_RETIRED_ADD_DOUBLE_FLOPS:PMC0,SSE_RETIRED_MULT_DOUBLE_FLOPS:PMC1,CPU_CLOCKS_UNHALTED:PMC2"},
    {"FLOPS_SP",FLOPS_SP,"Single Precision MFlops/s","SSE_RETIRED_ADD_SINLGE_FLOPS:PMC0,SSE_RETIRED_MULT_SINLGE_FLOPS:PMC1,CPU_CLOCKS_UNHALTED:PMC2"},
    {"L2",L2,"L2: L2 cache bandwidth in MBytes/s","DATA_CACHE_REFILLS_L2_ALL:PMC0,DATA_CACHE_EVICTED_ALL:PMC1,CPU_CLOCKS_UNHALTED:PMC2"},
    {"L3",L3,"L3: L3 cache bandwidth in MBytes/s","L3_FILLS_ALL_ALL_CORES:PMC0,L3_READ_REQUEST_ALL_ALL_CORES:PMC1,CPU_CLOCKS_UNHALTED:PMC2"},
    {"MEM",MEM,"Main memory bandwidth in MBytes/s",""},
    {"DATA",DATA,"Load to store ratio","INST_RETIRED_LOADS:PMC0,INST_RETIRED_STORES:PMC1,CPU_CLOCKS_UNHALTED:PMC2"},
    {"BRANCH",BRANCH,"Branch prediction miss rate","BRANCH_RETIRED:PMC0,BRANCH_MISPREDICT_RETIRED:PMC1,CPU_CLOCKS_UNHALTED:PMC2"},
    {"CPI",CPI,"CPI: cycles per instruction","UOPS_RETIRED:PMC0,CPU_CLOCKS_UNHALTED:PMC1"},
    {"TLB",TLB,"Translation lookaside buffer miss rate","DTLB_L2_MISS_4K:PMC0,CPU_CLOCKS_UNHALTED:PMC1"}
};


void perfmon_init_k10(PerfmonThread *thread)
{
    uint64_t flags = 0x0ULL;
    int cpu_id = thread->cpu_id;

    thread->counters[PMC0].configRegister = MSR_AMD_PERFEVTSEL0;
    thread->counters[PMC0].counterRegister = MSR_AMD_PMC0;
    thread->counters[PMC0].type = PMC;
    thread->counters[PMC1].configRegister = MSR_AMD_PERFEVTSEL1;
    thread->counters[PMC1].counterRegister = MSR_AMD_PMC1;
    thread->counters[PMC0].type = PMC;
    thread->counters[PMC2].configRegister = MSR_AMD_PERFEVTSEL2;
    thread->counters[PMC2].counterRegister = MSR_AMD_PMC2;
    thread->counters[PMC0].type = PMC;
    thread->counters[PMC3].configRegister = MSR_AMD_PERFEVTSEL3;
    thread->counters[PMC3].counterRegister = MSR_AMD_PMC3;
    thread->counters[PMC0].type = PMC;

    msr_write(cpu_id, MSR_AMD_PERFEVTSEL0, 0x0ULL);
    msr_write(cpu_id, MSR_AMD_PERFEVTSEL1, 0x0ULL);
    msr_write(cpu_id, MSR_AMD_PERFEVTSEL2, 0x0ULL);
    msr_write(cpu_id, MSR_AMD_PERFEVTSEL3, 0x0ULL);

    flags |= (1<<16);  /* user mode flag */

    msr_write(cpu_id, MSR_AMD_PERFEVTSEL0, flags);
    msr_write(cpu_id, MSR_AMD_PERFEVTSEL1, flags);
    msr_write(cpu_id, MSR_AMD_PERFEVTSEL2, flags);
    msr_write(cpu_id, MSR_AMD_PERFEVTSEL3, flags);
}


void
perfmon_setupCounterThread_k10(int thread_id,
        uint32_t event, uint32_t umask,
        PerfmonCounterIndex index)
{
    uint64_t flags;
    uint64_t reg = perfmon_threadData[thread_id].counters[index].config_reg;
    int cpu_id = perfmon_threadData[thread_id].cpu_id;
    perfmon_threadData[thread_id].counters[index].init = TRUE;

    flags = msr_read(cpu_id,reg);
    flags &= ~(0xFFFFU); 

	/* AMD uses a 12 bit Event mask: [35:32][7:0] */
	flags |= ((uint64_t)(event>>8)<<32) + (umask<<8) + (event & ~(0xF00U));

    if (perfmon_verbose)
    {
        printf("[%d] perfmon_setup_counter: Write Register 0x%llX , Flags: 0x%llX \n",
               cpu_id,
               LLU_CAST reg,
               LLU_CAST flags);
    }
    msr_write(cpu_id, reg , flags);
}


void
perfmon_startCountersThread_k10(int thread_id)
{
    int i;
    uint64_t flags;
    int cpu_id = perfmon_threadData[thread_id].cpu_id;

    for (i=0;i<NUM_COUNTERS_K10;i++) 
    {
        if (perfmon_threadData[thread_id].counters[i].init == TRUE) 
        {
            msr_write(cpu_id, perfmon_threadData[thread_id].counters[i].counter_reg , 0x0ULL);
            flags = msr_read(cpu_id,perfmon_threadData[thread_id].counters[i].config_reg);
            flags |= (1<<22);  /* enable flag */
            if (perfmon_verbose) 
            {
                printf("perfmon_start_counters: Write Register 0x%llX , Flags: 0x%llX \n",
                        LLU_CAST perfmon_threadData[thread_id].counters[i].config_reg,
                        LLU_CAST flags);
            }

            msr_write(cpu_id, perfmon_threadData[thread_id].counters[i].config_reg , flags);
        }
    }
}

void 
perfmon_stopCountersThread_k10(int thread_id)
{
    uint64_t flags;
    int i;
    int cpu_id = perfmon_threadData[thread_id].cpu_id;

    for (i=0;i<NUM_COUNTERS_K10;i++) 
    {
        if (perfmon_threadData[thread_id].counters[i].init == TRUE) 
        {
            flags = msr_read(cpu_id,perfmon_threadData[thread_id].counters[i].config_reg);
            flags &= ~(1<<22);  /* clear enable flag */
            msr_write(cpu_id, perfmon_threadData[thread_id].counters[i].config_reg , flags);
            if (perfmon_verbose)
            {
                printf("perfmon_stop_counters: Write Register 0x%llX , Flags: 0x%llX \n",
                        LLU_CAST perfmon_threadData[thread_id].counters[i].config_reg,
                        LLU_CAST flags);
            }
            perfmon_threadData[thread_id].pc[i] = msr_read(cpu_id, perfmon_threadData[thread_id].counters[i].counter_reg);
        }
    }
    perfmon_threadData[thread_id].cycles = perfmon_threadData[thread_id].pc[2];
}



void perfmon_printDerivedMetrics_k10(PerfmonGroup group)
{
    int threadId;
    double time = 0.0;
    double inverseClock = 1.0 /(double) timer_getCpuClock();
    PerfmonResultTable tableData;
    int numRows;
    int numColumns = perfmon_numThreads;
    bstrList* fc;
    bstring label;

    switch ( group )
    {
        case FLOPS_DP:
            numRows = 4;
            INIT_BASIC;
            bstrListAdd(1,Runtime [s]);
            bstrListAdd(2,DP MFlops/s);
            bstrListAdd(3,DP Add MFlops/s);
            bstrListAdd(4,DP Mult MFlops/s);
            initResultTable(&tableData, fc, numRows, numColumns);

            for(threadId=0; threadId < perfmon_numThreads; threadId++)
            {
                time = perfmon_getResult(threadId,"PMC2") * inverseClock;
                tableData.rows[0].value[threadId] = time;
                tableData.rows[1].value[threadId] =
                     1.0E-06*(perfmon_getResult(threadId,"PMC0")+ perfmon_getResult(threadId,"PMC1")) / time;
                tableData.rows[2].value[threadId] =
                     1.0E-06*(perfmon_getResult(threadId,"PMC0")) / time;
                tableData.rows[3].value[threadId] =
                     1.0E-06*(perfmon_getResult(threadId,"PMC1")) / time;
            }
            break;

        case FLOPS_SP:
            numRows = 4;
            INIT_BASIC;
            bstrListAdd(1,Runtime [s]);
            bstrListAdd(2,SP MFlops/s);
            bstrListAdd(3,SP Add MFlops/s);
            bstrListAdd(4,SP Mult MFlops/s);
            initResultTable(&tableData, fc, numRows, numColumns);

            for(threadId=0; threadId < perfmon_numThreads; threadId++)
            {
                time = perfmon_getResult(threadId,"PMC2") * inverseClock;
                tableData.rows[0].value[threadId] = time;
                tableData.rows[1].value[threadId] =
                     1.0E-06*(perfmon_getResult(threadId,"PMC0")+ perfmon_getResult(threadId,"PMC1")) / time;
                tableData.rows[2].value[threadId] =
                     1.0E-06*(perfmon_getResult(threadId,"PMC0")) / time;
                tableData.rows[3].value[threadId] =
                     1.0E-06*(perfmon_getResult(threadId,"PMC1")) / time;
            }
            break;

        case L2:
            numRows = 4;
            INIT_BASIC;
            bstrListAdd(1,Runtime [s]);
            bstrListAdd(2,L2 bandwidth MBytes/s);
            bstrListAdd(3,L2 refill bandwidth MBytes/s);
            bstrListAdd(4,L2 evict MBytes/s);
            initResultTable(&tableData, fc, numRows, numColumns);

            for(threadId=0; threadId < perfmon_numThreads; threadId++)
            {
                time = perfmon_getResult(threadId,"PMC2") * inverseClock;
                tableData.rows[0].value[threadId] = time;
                tableData.rows[1].value[threadId] =
                     1.0E-06*(perfmon_getResult(threadId,"PMC0")+ perfmon_getResult(threadId,"PMC1"))*64.0 / time;
                tableData.rows[2].value[threadId] =
                     1.0E-06*(perfmon_getResult(threadId,"PMC0")*64.0) / time;
                tableData.rows[3].value[threadId] =
                     1.0E-06*(perfmon_getResult(threadId,"PMC1")*64.0) / time;
            }
            break;

        case L3:
            numRows = 4;
            INIT_BASIC;
            bstrListAdd(1,Runtime [s]);
            bstrListAdd(2,L3 bandwidth MBytes/s);
            bstrListAdd(3,L3 evict bandwidth MBytes/s);
            bstrListAdd(4,L3 read MBytes/s);
            initResultTable(&tableData, fc, numRows, numColumns);

            for(threadId=0; threadId < perfmon_numThreads; threadId++)
            {
                time = perfmon_getResult(threadId,"PMC2") * inverseClock;
                tableData.rows[0].value[threadId] = time;
                tableData.rows[1].value[threadId] =
                     1.0E-06*(perfmon_getResult(threadId,"PMC0")+ perfmon_getResult(threadId,"PMC1"))*64.0 / time;
                tableData.rows[2].value[threadId] =
                     1.0E-06*(perfmon_getResult(threadId,"PMC0")*64.0) / time;
                tableData.rows[3].value[threadId] =
                     1.0E-06*(perfmon_getResult(threadId,"PMC1")*64.0) / time;
            }
            break;

        case MEM:
            break;

        case DATA:
            numRows = 2;
            INIT_BASIC;
            bstrListAdd(1,Runtime [s]);
            bstrListAdd(2,Store to Load ratio);
            initResultTable(&tableData, fc, numRows, numColumns);

            for(threadId=0; threadId < perfmon_numThreads; threadId++)
            {
                time = perfmon_getResult(threadId,"PMC2") * inverseClock;
                tableData.rows[0].value[threadId] = time;
                tableData.rows[1].value[threadId] =
                     (perfmon_getResult(threadId,"PMC0")/ perfmon_getResult(threadId,"PMC1"));
            }
            break;

        case BRANCH:
            numRows = 2;
            INIT_BASIC;
            bstrListAdd(1,Runtime [s]);
            bstrListAdd(2,Ratio Mispredicted Branches);
            initResultTable(&tableData, fc, numRows, numColumns);

            for(threadId=0; threadId < perfmon_numThreads; threadId++)
            {
                time = perfmon_getResult(threadId,"PMC2") * inverseClock;
                tableData.rows[0].value[threadId] = time;
                tableData.rows[1].value[threadId] =
                     (perfmon_getResult(threadId,"PMC1")/ perfmon_getResult(threadId,"PMC0"));
            }
            break;

        case CPI:
            numRows = 3;
            INIT_BASIC;
            bstrListAdd(1,Runtime [s]);
            bstrListAdd(2,uops retired);
            bstrListAdd(2,CPI);
            initResultTable(&tableData, fc, numRows, numColumns);

            for(threadId=0; threadId < perfmon_numThreads; threadId++)
            {
                time = perfmon_getResult(threadId,"PMC1") * inverseClock;
                tableData.rows[0].value[threadId] = time;
                tableData.rows[1].value[threadId] =
                     perfmon_getResult(threadId,"PMC0");
                tableData.rows[2].value[threadId] =
                     (perfmon_getResult(threadId,"PMC1")/ perfmon_getResult(threadId,"PMC0"));
            }
            break;

        case TLB:
            numRows = 2;
            INIT_BASIC;
            bstrListAdd(1,Runtime [s]);
            bstrListAdd(2,TLB misses);
            initResultTable(&tableData, fc, numRows, numColumns);

            for(threadId=0; threadId < perfmon_numThreads; threadId++)
            {
                time = perfmon_getResult(threadId,"PMC1") * inverseClock;
                tableData.rows[0].value[threadId] = time;
                tableData.rows[1].value[threadId] =
                     perfmon_getResult(threadId,"PMC0");
            }
            break;

        case NOGROUP:
            numRows = 2;
            INIT_BASIC;
            bstrListAdd(1,Runtime [s]);
            bstrListAdd(2,CPI);
            initResultTable(&tableData, fc, numRows, numColumns);

            for(threadId=0; threadId < perfmon_numThreads; threadId++)
            {
                time = perfmon_getResult(threadId,"PMC0") * inverseClock;
                if (perfmon_getResult(threadId,"PMC1") < 1.0E-12)
                {
                    cpi  =  0.0;
                }
                else
                {
                    cpi  =  perfmon_getResult(threadId,"PMC0")/
                        perfmon_getResult(threadId,"PMC1");
                }

                tableData.rows[0].value[threadId] = time;
                tableData.rows[1].value[threadId] = cpi;
            }

            break;

        default:
            fprintf (stderr, "perfmon_printDerivedMetricsCore2: Unknown group! Exiting!\n" );
            exit (EXIT_FAILURE);
            break;
    }

    printResultTable(&tableData);
    bdestroy(label);
    bstrListDestroy(fc);
}

