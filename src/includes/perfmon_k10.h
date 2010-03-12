/*
 * ===========================================================================
 *
 *       Filename:  perfmon_k10.h
 *
 *    Description:  K10 specific subroutines
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


int
perfmon_getIndex_k10 (bstring str, PerfmonCounterIndex* index)
{
   if (biseqcstr(str,"PMC0"))
   {
       *index = PMC0;
   }
   else if (biseqcstr(str,"PMC1"))
   {
       *index = PMC1;
   }
   else if (biseqcstr(str,"PMC2"))
   {
       *index = PMC2;
   }
   else if (biseqcstr(str,"PMC3"))
   {
       *index = PMC3;
   }
   else
   {
       return FALSE;
   }

   return TRUE;
}


void perfmon_init_k10(PerfmonThread *thread)
{
    uint64_t flags = 0x0ULL;
    int cpu_id = thread->cpu_id;

    thread->counters[PMC0].config_reg = MSR_AMD_PERFEVTSEL0;
    thread->counters[PMC0].counter_reg = MSR_AMD_PMC0;
    thread->counters[PMC1].config_reg = MSR_AMD_PERFEVTSEL1;
    thread->counters[PMC1].counter_reg = MSR_AMD_PMC1;
    thread->counters[PMC2].config_reg = MSR_AMD_PERFEVTSEL2;
    thread->counters[PMC2].counter_reg = MSR_AMD_PMC2;
    thread->counters[PMC3].config_reg = MSR_AMD_PERFEVTSEL3;
    thread->counters[PMC3].counter_reg = MSR_AMD_PMC3;

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
perfmon_setupReport_k10(MultiplexCollections* collections)
{

}

void
perfmon_printReport_k10(MultiplexCollections* collections)
{

}

void
perfmon_printGroups_k10 (void)
{


}

PerfmonGroup
perfmon_getGroupId_k10 (bstring groupStr)
{
	PerfmonGroup group;

	if (biseqcstr(groupStr,"FLOPS_DP")) 
	{
		group = FLOPS_DP;
	}
	else if (biseqcstr(groupStr,"FLOPS_SP")) 
	{
		group = FLOPS_SP;
	}
	else if (biseqcstr(groupStr,"L2")) 
	{
		group = L2;
	}
	else if (biseqcstr(groupStr,"MEM")) 
	{
		group = MEM;
	}
	else if (biseqcstr(groupStr,"DATA")) 
	{
		group = DATA;
	}
	else if (biseqcstr(groupStr,"BRANCH")) 
	{
		group = BRANCH;
	}
	else if (biseqcstr(groupStr,"TLB")) 
	{
		group = TLB;
	}
	else if (biseqcstr(groupStr,"CPI")) 
	{
		group = CPI;
	}
	else
	{
		return NOGROUP;
	}

	return group;
}

void
perfmon_setupCounterThread_k10(int thread_id,
        uint32_t event, uint32_t umask,
        PerfmonCounterIndex index)
{
    uint64_t flags;
    uint64_t reg = threadData[thread_id].counters[index].config_reg;
    int cpu_id = threadData[thread_id].cpu_id;
    threadData[thread_id].counters[index].init = TRUE;

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
    int cpu_id = threadData[thread_id].cpu_id;

    for (i=0;i<NUM_PMC;i++) 
    {
        if (threadData[thread_id].counters[i].init == TRUE) 
        {
            msr_write(cpu_id, threadData[thread_id].counters[i].counter_reg , 0x0ULL);
            flags = msr_read(cpu_id,threadData[thread_id].counters[i].config_reg);
            flags |= (1<<22);  /* enable flag */
            if (perfmon_verbose) 
            {
                printf("perfmon_start_counters: Write Register 0x%llX , Flags: 0x%llX \n",
                        LLU_CAST threadData[thread_id].counters[i].config_reg,
                        LLU_CAST flags);
            }

            msr_write(cpu_id, threadData[thread_id].counters[i].config_reg , flags);
        }
    }
}

void 
perfmon_stopCountersThread_k10(int thread_id)
{
    uint64_t flags;
    int i;
    int cpu_id = threadData[thread_id].cpu_id;

    for (i=0;i<NUM_PMC;i++) 
    {
        if (threadData[thread_id].counters[i].init == TRUE) 
        {
            flags = msr_read(cpu_id,threadData[thread_id].counters[i].config_reg);
            flags &= ~(1<<22);  /* clear enable flag */
            msr_write(cpu_id, threadData[thread_id].counters[i].config_reg , flags);
            if (perfmon_verbose)
            {
                printf("perfmon_stop_counters: Write Register 0x%llX , Flags: 0x%llX \n",
                        LLU_CAST threadData[thread_id].counters[i].config_reg,
                        LLU_CAST flags);
            }
            threadData[thread_id].pc[i] = msr_read(cpu_id, threadData[thread_id].counters[i].counter_reg);
        }
    }
    threadData[thread_id].cycles = threadData[thread_id].pc[2];
}


void
perfmon_setupGroupThread_k10(int thread_id,PerfmonGroup group)
{
    bstring event_0 = bformat("NOINIT");
    bstring event_1 = bformat("NOINIT");
    bstring event_2 = bformat("NOINIT");
    bstring event_3 = bformat("NOINIT");

    switch ( group ) {
        case FLOPS_DP:
            bassigncstr(event_0, "SSE_RETIRED_ADD_DOUBLE_FLOPS");
            bassigncstr(event_1, "SSE_RETIRED_MULT_DOUBLE_FLOPS");
            bassigncstr(event_2, "CPU_CLOCKS_UNHALTED");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            setupCounterThread(thread_id, PMC2, event_2);
            break;

        case FLOPS_SP:
            bassigncstr(event_0, "SSE_RETIRED_ADD_SINGLE_FLOPS");
            bassigncstr(event_1, "SSE_RETIRED_MULT_SINGLE_FLOPS");
            bassigncstr(event_2, "CPU_CLOCKS_UNHALTED");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            setupCounterThread(thread_id, PMC2, event_2);
            break;

        case L1:
            bassigncstr(event_0, "DATA_CACHE_REFILLS_L2_ALL");
            bassigncstr(event_1, "DATA_CACHE_EVICTED_ALL");
            bassigncstr(event_2, "CPU_CLOCKS_UNHALTED");
            bassigncstr(event_3, "DATA_CACHE_REFILLS_NORTHBRIDGE_ALL");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            setupCounterThread(thread_id, PMC2, event_2);
            setupCounterThread(thread_id, PMC3, event_3);
            break;

        case L2:
            bassigncstr(event_0, "DATA_CACHE_REFILLS_L2_ALL");
            bassigncstr(event_1, "DATA_CACHE_EVICTED_ALL");
            bassigncstr(event_2, "CPU_CLOCKS_UNHALTED");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            setupCounterThread(thread_id, PMC2, event_2);
            break;

        case L3:
            bassigncstr(event_0, "L3_FILLS_ALL_ALL_CORES");
            bassigncstr(event_1, "L3_READ_REQUEST_ALL_ALL_CORES");
            bassigncstr(event_2, "CPU_CLOCKS_UNHALTED");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            setupCounterThread(thread_id, PMC2, event_2);
            break;

        case MEM:
            break;

        case DATA:
            bassigncstr(event_0, "INST_RETIRED_LOADS");
            bassigncstr(event_1, "INST_RETIRED_STORES");
            bassigncstr(event_2, "CPU_CLOCKS_UNHALTED");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            setupCounterThread(thread_id, PMC2, event_2);
            break;

        case BRANCH:
            bassigncstr(event_0, "BRANCH_RETIRED");
            bassigncstr(event_1, "BRANCH_MISPREDICT_RETIRED");
            bassigncstr(event_2, "CPU_CLOCKS_UNHALTED");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            setupCounterThread(thread_id, PMC2, event_2);
            break;

        case CPI:
            bassigncstr(event_0, "UOPS_RETIRED");
            bassigncstr(event_1, "CPU_CLOCKS_UNHALTED");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            break;

        case TLB:
            bassigncstr(event_0, "DTLB_L2_MISS_4K");
            bassigncstr(event_1, "CPU_CLOCKS_UNHALTED");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            break;

        default:
            break;
    }

    bdestroy(event_0);
    bdestroy(event_1);
    bdestroy(event_2);
    bdestroy(event_3);
}


void perfmon_printResults_k10(PerfmonThread *thread, PerfmonGroup group_set, float time)
{
    int cpu_id = thread->cpu_id;

    switch ( group_set ) {
        case FLOPS_DP:
            printf ("[%d] Double MFlops/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[0]+thread->pc[1])/time);
            printf ("[%d] Double Add MFlops/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[0])/time);
            printf ("[%d] Double Mult MFlops/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[1])/time);
            break;

        case FLOPS_SP:
            printf ("[%d] Single MFlops/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[0]+thread->pc[1])/time);
            printf ("[%d] Single Add MFlops/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[0])/time);
            printf ("[%d] Single Mult MFlops/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[1])/time);
            break;

        case L1:
            printf ("[%d] L1 bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[0]+thread->pc[1]+thread->pc[3])*64)/time);
            printf ("[%d] L2 refill bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[0]*64)/time);
            printf ("[%d] L2 evict MBytes/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[1]*64)/time);
            printf ("[%d] Northbridge refill MBytes/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[3]*64)/time);
            break;

        case L2:
            printf ("[%d] L2 bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[0]+thread->pc[1])*64)/time);
            printf ("[%d] L2 refill bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[0]*64)/time);
            printf ("[%d] L2 evict MBytes/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[1]*64)/time);
            break;

        case L3:
            printf ("[%d] L3 bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[0]+thread->pc[1])*64)/time);
            printf ("[%d] L3 evict from L2 bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[0]*64)/time);
            printf ("[%d] L2 read MBytes/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[1]*64)/time);
            break;

        case MEM:
            break;

        case DATA:
            printf ("[%d] Store to Load ratio: 1:%f \n",cpu_id,(float)thread->pc[0]/(float)thread->pc[1]);
            break;

        case BRANCH:
            printf ("[%d] Ratio Mispredicted Branches: %f \n",cpu_id,(float)thread->pc[1]/(float)thread->pc[0]);
            break;

        case CPI:
            printf ("[%d] Cycles per uop/s: %f \n",cpu_id,(float)thread->cycles/(float)thread->pc[0]);
            break;


        case TLB:
            break;

        default:
            break;
    }

}

