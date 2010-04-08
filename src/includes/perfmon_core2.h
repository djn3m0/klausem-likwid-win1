/*
 * ===========================================================================
 *
 *       Filename:  perfmon_core2.h
 *
 *    Description:  Core 2 specific subroutines
 *
 *        Version:  <VERSION>
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

#define NUM_COUNTERS_CORE2 4
#define NUM_GROUPS_CORE2 7

static PerfmonCounterMap core2_counter_map[NUM_COUNTERS_CORE2] = {
    {"PMC0",PMC2},
    {"PMC1",PMC3},
    {"FIXC0",PMC0},
    {"FIXC1",PMC1}
};

static PerfmonGroupMap core2_group_map[NUM_GROUPS_CORE2] = {
    {"FLOPS_DP",FLOPS_DP,"Double Precision MFlops/s"},
    {"FLOPS_SP",FLOPS_SP,"Single Precision MFlops/s"},
    {"L2",L2,"L2: L2 cache bandwidth in MBytes/s"},
    {"MEM",MEM,"Main memory bandwidth in MBytes/s"},
    {"DATA",DATA,"Load to store ratio"},
    {"BRANCH",BRANCH,"Branch prediction miss rate"},
    {"TLB",TLB,"Translation lookaside buffer miss rate"}
};

static char** core2_group_config[NUM_GROUPS_CORE2] = {
    "INSTR_RETIRED_ANY:FIXC0,CPU_CLK_UNHALTED_CORE:FIXC1,SIMD_COMP_INST_RETIRED_PACKED_DOUBLE:PMC0,SIMD_COMP_INST_RETIRED_SCALAR_DOUBLE:PMC1",
    "INSTR_RETIRED_ANY:FIXC0,CPU_CLK_UNHALTED_CORE:FIXC1,SIMD_COMP_INST_RETIRED_PACKED_SINGLE:PMC0,SIMD_COMP_INST_RETIRED_SCALAR_SINGLE:PMC1",
    "INSTR_RETIRED_ANY:FIXC0,CPU_CLK_UNHALTED_CORE:FIXC1,L1D_REPL:PMC0,L1D_M_EVICT:PMC1",
    "INSTR_RETIRED_ANY:FIXC0,CPU_CLK_UNHALTED_CORE:FIXC1,BUS_TRANS_MEM_THIS_CORE_THIS_A:PMC0",
    "INSTR_RETIRED_ANY:FIXC0,CPU_CLK_UNHALTED_CORE:FIXC1,INST_RETIRED_LOADS:PMC0,INST_RETIRED_STORES:PMC1",
    "INSTR_RETIRED_ANY:FIXC0,CPU_CLK_UNHALTED_CORE:FIXC1,BR_INST_RETIRED_ANY:PMC0,BR_INST_RETIRED_MISPRED:PMC1",
    "INSTR_RETIRED_ANY:FIXC0,CPU_CLK_UNHALTED_CORE:FIXC1,UOPS_RETIRED_ANY:PMC0",
    "INSTR_RETIRED_ANY:FIXC0,CPU_CLK_UNHALTED_CORE:FIXC1,DTLB_MISSES_ANY:PMC0,DTLB_MISSES_MISS_LD:PMC1",
    "INSTR_RETIRED_ANY:FIXC0,CPU_CLK_UNHALTED_CORE:FIXC1,UOPS_RETIRED_ANY:PMC0,DTLB_MISSES_MISS_LD:PMC1"};


void 
perfmon_init_core2(PerfmonThread *thread)
{
    uint64_t flags = 0x0ULL;
    int cpu_id = thread->cpu_id;

    /* Fixed Counters: instructions retired, cycles ubhalted core */
    thread->counters[PMC0].configRegister = MSR_PERF_FIXED_CTR_CTRL;
    thread->counters[PMC0].counterRegister = MSR_PERF_FIXED_CTR0;
    thread->counters[PMC0].type = FIXED;
    thread->counters[PMC1].configRegister = MSR_PERF_FIXED_CTR_CTRL;
    thread->counters[PMC1].counterRegister = MSR_PERF_FIXED_CTR1;
    thread->counters[PMC1].type = FIXED;

    /* PMC Counters: 2 40bit wide */
    thread->counters[PMC2].configRegister = MSR_PERFEVTSEL0;
    thread->counters[PMC2].counterRegister = MSR_PMC0;
    thread->counters[PMC2].type = PMC;
    thread->counters[PMC3].configRegister = MSR_PERFEVTSEL1;
    thread->counters[PMC3].counterRegister = MSR_PMC1;
    thread->counters[PMC3].type = PMC;

    /* Initialize registers */
    msr_write(cpu_id, MSR_PERFEVTSEL0, 0x0ULL);
    msr_write(cpu_id, MSR_PERFEVTSEL1, 0x0ULL);

    msr_write(cpu_id, MSR_PERF_FIXED_CTR_CTRL, 0x0ULL);
    msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);
    msr_write(cpu_id, MSR_PERF_GLOBAL_OVF_CTRL, 0x0ULL);
    msr_write(cpu_id, MSR_PEBS_ENABLE, 0x0ULL);

    /* always initialize fixed counters
     * FIXED 0: Instructions retired
     * FIXED 1: Clocks unhalted */
    msr_write(cpu_id, MSR_PERF_FIXED_CTR_CTRL, 0x22ULL);

    /* Preinit of two PMC counters */
    flags |= (1<<16);  /* user mode flag */
    flags |= (1<<19);  /* pin control flag */
    flags |= (1<<22);  /* enable flag */

    msr_write(cpu_id, MSR_PERFEVTSEL0, flags);
    msr_write(cpu_id, MSR_PERFEVTSEL1, flags);
}


void
perfmon_setupCounterThread_core2(int thread_id,
        uint32_t event, uint32_t umask,
        PerfmonCounterIndex index)
{
    uint64_t flags;
    uint64_t reg = threadData[thread_id].counters[index].configRegister;
    int cpu_id = threadData[thread_id].cpu_id;

    /* only the PMC counters need to be set up on Core 2 */
    if (threadData[thread_id].counters[index].type == PMC)
    {

        threadData[thread_id].counters[index].init = TRUE;
        flags = msr_read(cpu_id,reg);
        flags &= ~(0xFFFFU); 

        /* Intel with standard 8 bit event mask: [7:0] */
        flags |= (umask<<8) + event;

        msr_write(cpu_id, reg , flags);

        if (perfmon_verbose)
        {
            printf("[%d] perfmon_setup_counter: Write Register 0x%llX , Flags: 0x%llX \n",
                    cpu_id,
                    LLU_CAST reg,
                    LLU_CAST flags);
        }
    }
}

void
perfmon_startCountersThread_core2(int thread_id)
{
    int i;
    uint64_t flags;
    int cpu_id = threadData[thread_id].cpu_id;

    msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);
    msr_write(cpu_id, MSR_PERF_FIXED_CTR0, 0x0ULL);
    msr_write(cpu_id, MSR_PERF_FIXED_CTR1, 0x0ULL);

    /* Enable fixed counters */
    flags  = 0x300000000ULL;

    for (i=0;i<NUM_COUNTERS_CORE2;i++) {
        if (threadData[thread_id].counters[i].init == TRUE) {
            msr_write(cpu_id, threadData[thread_id].counters[i].counterRegister , 0x0ULL);
            flags |= (1<<i);  /* enable counter */
        }
    }

    if (perfmon_verbose)
    {
        printf("perfmon_start_counters: Write Register 0x%X , Flags: 0x%llX \n",
                MSR_PERF_GLOBAL_CTRL, LLU_CAST flags);
    }

    msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, flags);
    msr_write(cpu_id, MSR_PERF_GLOBAL_OVF_CTRL, 0x300000003ULL);
}

void 
perfmon_stopCountersThread_core2(int thread_id)
{
    uint64_t flags;
    int i;
    int cpu_id = threadData[thread_id].cpu_id;

    /* stop counters */
    msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);

    /* read out counter results */
    for (i=0;i<NUM_COUNTERS_CORE2;i++) 
    {
        if (threadData[thread_id].counters[i].init == TRUE) 
        {
            threadData[thread_id].counters[i].counterData =
                msr_read(cpu_id, threadData[thread_id].counters[i].counterRegister);
        }
    }

    /* check overflow status */
    flags = msr_read(cpu_id,MSR_PERF_GLOBAL_STATUS);
    if((flags & 0x3) || (flags & (0x3ULL<<32)) ) 
    {
        printf ("Overflow occured \n");
        printf ("Status: 0x%llX \n", LLU_CAST flags);
    }
}

void
perfmon_printResults_core2(PerfmonGroup group)
{
    TableContainer* table;
    bstrList* labelStrings;
    int numRows = 1; /* Default Row CPI */
    int numColumns = perfmon_numThreads+1; /* Default Row CPI */

    labelStrings = bstrListCreate();
    bstrListAlloc(labelStrings, numColumns);

    label = bfromcstr("Metric");
    labelStrings->entry[0] = bstrcpy(label);
    labelStrings->qty++;

    for (i=0; i<perfmon_numThreads;i++)
    {
        label = bformat("core %d",threadData[i].cpu_id);
        labelStrings->entry[1+i] = bstrcpy(label);
        labelStrings->qty++;
    }

    printf ("[%d] Cycles per uop/s: %f \n",cpu_id,(float)thread->cycles/(float)thread->instructionsRetired);

    switch ( group ) 
    {
        case FLOPS_DP:
            numRows += 1;

            if (time < 1.0E-12)
            {
                printf ("[%d] Double Precision MFlops/s: %f \n",
                        cpu_id,0.0);
            }
            else
            {
                printf ("[%d] Double Precision MFlops/s: %f \n",
                        cpu_id,1.0E-06*(float)(thread->pc[0]*2+thread->pc[1])/time);
            }
            break;

        case FLOPS_SP:
            if (time < 1.0E-12)
            {
                printf ("[%d] Single Precision MFlops/s: %f \n",
                        cpu_id,0.0);
            }
            else
            {
                printf ("[%d] Single Precision MFlops/s: %f \n",
                        cpu_id,1.0E-06*(float)(thread->pc[0]*4+thread->pc[1])/time);
            }
            break;

        case L2:
            if (time < 1.0E-12)
            {
                printf ("[%d] L2 Load MBytes/s: %f \n",cpu_id,0.0);
                printf ("[%d] L2 Evict MBytes/s: %f \n",cpu_id,0.0);
                printf ("[%d] L2 bandwidth MBytes/s: %f \n",cpu_id,0.0);
            }
            else
            {
                printf ("[%d] L2 Load MBytes/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[0]*64)/time);
                printf ("[%d] L2 Evict MBytes/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[1]*64)/time);
                printf ("[%d] L2 bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[0]+thread->pc[1])*64)/time);
            }
            break;

        case MEM:
            if (time < 1.0E-12)
            {
                printf ("[%d] Memory bandwidth MBytes/s: %f \n",cpu_id,0.0);
            }
            else
            {
                printf ("[%d] Memory bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[0]*64)/time);
            }
            break;

        case DATA:
            printf ("[%d] Store to Load ratio: 1:%f \n",cpu_id,(float)thread->pc[0]/(float)thread->pc[1]);
            break;

        case BRANCH:
            printf ("[%d] Mispredicted Branches: %f \n",cpu_id,(float) (thread->pc[1]/(float)thread->pc[0]) * 100);
            break;

        case CPI:
            printf ("[%d] Cycles per uop/s: %f \n",cpu_id,(float)thread->cycles/(float)thread->pc[0]);
            break;

        case FRONTEND:
            printf ("[%d] Instruction starvation: %f \n",cpu_id,(float)thread->cycles/(float)thread->pc[0]);
            break;

        default:
            break;
    }

    table = asciiTable_allocate(numRows, numColumns,labelStrings);

}


void
perfmon_setupReport_core2(MultiplexCollections* collections)
{
#if 0
    collections->numberOfCollections = 6;
    collections->collections =
		(PerfmonEventSet*) malloc(collections->numberOfCollections
				* sizeof(PerfmonEventSet));

    /*  Load/Store */
    collections->collections[0].numberOfEvents = 2;

    collections->collections[0].events =
		(PerfmonEventSetEntry*) malloc(2 * sizeof(PerfmonEventSetEntry));

    collections->collections[0].events[0].eventName =
		bfromcstr("INST_RETIRED_LOADS");

    collections->collections[0].events[0].reg =
		bfromcstr("PMC0");

    collections->collections[0].events[1].eventName =
		bfromcstr("INST_RETIRED_STORES");

    collections->collections[0].events[1].reg =
		bfromcstr("PMC1");

    /*  Branches */
    collections->collections[1].numberOfEvents = 2;
    collections->collections[1].events =
		(PerfmonEventSetEntry*) malloc(2 * sizeof(PerfmonEventSetEntry));

    collections->collections[1].events[0].eventName =
		bfromcstr("BR_INST_RETIRED_ANY");

    collections->collections[1].events[0].reg =
		bfromcstr("PMC0");

    collections->collections[1].events[1].eventName =
		bfromcstr("BR_INST_RETIRED_MISPRED");

    collections->collections[1].events[1].reg =
		bfromcstr("PMC1");

    /*  SIMD Double */
    collections->collections[2].numberOfEvents = 2;
    collections->collections[2].events =
		(PerfmonEventSetEntry*) malloc(2 * sizeof(PerfmonEventSetEntry));

    collections->collections[2].events[0].eventName =
		bfromcstr("SIMD_INST_RETIRED_PACKED_DOUBLE");

    collections->collections[2].events[0].reg =
		bfromcstr("PMC0");

    collections->collections[2].events[1].eventName =
		bfromcstr("SIMD_INST_RETIRED_SCALAR_DOUBLE");

    collections->collections[2].events[1].reg =
		bfromcstr("PMC1");

    /*  L1 Utilization */
    collections->collections[3].numberOfEvents = 2;
    collections->collections[3].events = 
		(PerfmonEventSetEntry*) malloc(2 * sizeof(PerfmonEventSetEntry));

    collections->collections[3].events[0].eventName =
		bfromcstr("MEM_LOAD_RETIRED_L1D_LINE_MISS");

    collections->collections[3].events[0].reg =
		bfromcstr("PMC0");

    collections->collections[3].events[1].eventName =
		bfromcstr("L1D_ALL_REF");

    collections->collections[3].events[1].reg =
		bfromcstr("PMC1");

    /*  Memory transfers/TLB misses */
    collections->collections[4].numberOfEvents = 2;
    collections->collections[4].events =
		(PerfmonEventSetEntry*) malloc(2 * sizeof(PerfmonEventSetEntry));

    collections->collections[4].events[0].eventName =
		bfromcstr("BUS_TRANS_MEM_THIS_CORE_THIS_A");

    collections->collections[4].events[0].reg =
		bfromcstr("PMC0");

    collections->collections[4].events[1].eventName =
		bfromcstr("DTLB_MISSES_ANY");

    collections->collections[4].events[1].reg =
		bfromcstr("PMC1");

    /*  L2 bandwidth */
    collections->collections[5].numberOfEvents = 2;
    collections->collections[5].events =
		(PerfmonEventSetEntry*) malloc(2 * sizeof(PerfmonEventSetEntry));

    collections->collections[5].events[0].eventName =
		bfromcstr("L1D_REPL");

    collections->collections[5].events[0].reg =
		bfromcstr("PMC0");

    collections->collections[5].events[1].eventName =
		bfromcstr("L1D_M_EVICT");

    collections->collections[5].events[1].reg =
		bfromcstr("PMC1");
#endif
}

void
perfmon_printReport_core2(MultiplexCollections* collections)
{
#if 0
    printf(HLINE);
    printf("PERFORMANCE REPORT\n");
    printf(HLINE);
    printf("\nRuntime  %.2f s\n\n",collections->time);

    /* Section 1 */
    printf("Code characteristics:\n");
    printf("\tLoad to store ratio %f \n",
			collections->collections[0].events[0].results[0]/
			collections->collections[0].events[1].results[0]);

    printf("\tPercentage SIMD vectorized double %.2f \n",
			(collections->collections[2].events[0].results[0]*100.0)/
            (collections->collections[2].events[0].results[0]+
			 collections->collections[2].events[1].results[0]));

    printf("\tPercentage mispredicted branches  %.2f \n",
			(collections->collections[1].events[1].results[0]*100.0)/
            collections->collections[1].events[0].results[0]);

    /* Section 2 */
    printf("\nCode intensity:\n");
    printf("\tDouble precision Flops/s  %.2f MFlops/s\n",
			1.0E-06*(collections->collections[2].events[0].results[0]*2.0+
				collections->collections[2].events[1].results[0] )/
			(double) (collections->time*0.16666666));

    printf("\nResource Utilization:\n");
    printf("\tL1 Ref per miss %.2f \n",
			(collections->collections[3].events[1].results[0]/
			 collections->collections[3].events[0].results[0]));

    printf("\tRefs per TLB miss  %.2f \n",
			(collections->collections[3].events[1].results[0]/
			 collections->collections[4].events[1].results[0]));

    /* Section 3 */
    printf("\nBandwidths:\n");
    printf("\tL2 bandwidth  %.2f MBytes/s\n",
			1.0E-06*((collections->collections[5].events[1].results[0]+
					collections->collections[5].events[0].results[0])*64.0)/
			(double) (collections->time*0.16666666));

    printf("\tMemory bandwidth  %.2f MBytes/s\n",
			1.0E-06*((collections->collections[4].events[0].results[0])*64.0)/
			(double) (collections->time*0.16666666));
    printf(HLINE);
#endif
}



