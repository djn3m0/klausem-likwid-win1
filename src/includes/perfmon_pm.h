/*
 * ===========================================================================
 *
 *       Filename:  perfmon_pm.h
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


    void 
perfmon_init_pm(PerfmonThread *thread)
{
    uint64_t flags = 0x0ULL;
    int cpu_id = thread->cpu_id;

    thread->counters[PMC0].config_reg = MSR_PERFEVTSEL0;
    thread->counters[PMC0].counter_reg = MSR_PMC0;
    thread->counters[PMC1].config_reg = MSR_PERFEVTSEL1;
    thread->counters[PMC1].counter_reg = MSR_PMC1;

    msr_write(cpu_id, MSR_PERFEVTSEL0, 0x0ULL);
    msr_write(cpu_id, MSR_PERFEVTSEL1, 0x0ULL);

    /* Preinit of two PMC counters */
    flags |= (1<<16);  /* user mode flag */
    flags |= (1<<19);  /* pin control flag */
    //    flags |= (1<<22);  /* enable flag */

    msr_write(cpu_id, MSR_PERFEVTSEL0, flags);
    msr_write(cpu_id, MSR_PERFEVTSEL1, flags);
}

    void
perfmon_printGroups_pm (void)
{
    printf("Performance Groups: Pentium M\n\n");
    printf("FLOPS_DP: Double Precision MFlops/s\n");
    printf("FLOPS_SP: Single Precision MFlops/s\n");
    printf("L2: L2 cache bandwidth in MBytes/s\n");
    printf("BRANCH: Branch prediction miss rate\n");
    printf("CPI: cycles per instruction\n\n");
}


    void
perfmon_startCountersThread_pm(int thread_id)
{
    uint64_t flags;
    int cpu_id = threadData[thread_id].cpu_id;

    /* zero the counter registers */
    msr_write(cpu_id, threadData[thread_id].counters[0].counter_reg , 0x0ULL);
    msr_write(cpu_id, threadData[thread_id].counters[1].counter_reg , 0x0ULL);

    /* on p6 only MSR_PERFEVTSEL0 has the enable bit
     * it enables both counters as long MSR_PERFEVTSEL1 
     * has a valid configuration */
    flags = msr_read(cpu_id, MSR_PERFEVTSEL0);
    flags |= (1<<22);  /* enable flag */

    if (perfmon_verbose)
    {
        printf("perfmon_start_counters: Write Register 0x%X , Flags: 0x%llX \n",MSR_PERFEVTSEL0, LLU_CAST flags);
    }

    msr_write(cpu_id, MSR_PERFEVTSEL0, flags);

}

    void 
perfmon_stopCountersThread_pm(int thread_id)
{
    int i;
    int cpu_id = threadData[thread_id].cpu_id;

    msr_write(cpu_id, MSR_PERFEVTSEL0, 0x0ULL);
    msr_write(cpu_id, MSR_PERFEVTSEL1, 0x0ULL);


    for (i=0;i<NUM_PMC;i++) 
    {
        if (threadData[thread_id].counters[i].init == TRUE) 
        {
            threadData[thread_id].pc[i] = msr_read(cpu_id, threadData[thread_id].counters[i].counter_reg);
        }
    }

    /*
    threadData[thread_id].cycles = RDTSC;
    */
}




    PerfmonGroup
perfmon_getGroupId_pm (bstring groupStr)
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
    else if (biseqcstr(groupStr,"BRANCH")) 
    {
        group = BRANCH;
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




void perfmon_setupGroupThread_pm(int thread_id, PerfmonGroup group)
{
    bstring event_0 = bformat("NOINIT");
    bstring event_1 = bformat("NOINIT");

    switch ( group ) 
    {
        case FLOPS_DP:
            bassigncstr(event_0,"EMON_SSE_SSE2_COMP_INST_RETIRED_PACKED_DP");
            bassigncstr(event_1,"EMON_SSE_SSE2_COMP_INST_RETIRED_SCALAR_DP");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            break;

        case FLOPS_SP:
            bassigncstr(event_0,"EMON_SSE_SSE2_COMP_INST_RETIRED_ALL_SP");
            bassigncstr(event_1,"EMON_SSE_SSE2_COMP_INST_RETIRED_SCALAR_SP");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            break;

        case L2:
            bassigncstr(event_0,"L2_LINES_IN_ALL_ALL");
            bassigncstr(event_1,"L2_LINES_OUT_ALL_ALL");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            break;

        case BRANCH:
            bassigncstr(event_0,"BR_INST_EXEC");
            bassigncstr(event_1,"BR_INST_MISSP_EXEC");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            break;

        case CPI:
            bassigncstr(event_0,"UOPS_RETIRED");
            setupCounterThread(thread_id, PMC0, event_0);
            break;

        default:
            break;
    }

    bdestroy(event_0);
    bdestroy(event_1);
}

void perfmon_print_results_pm(PerfmonThread *thread, PerfmonGroup group_set, float time)
{
    int cpu_id = thread->cpu_id;

    switch ( group_set ) 
    {
        case FLOPS_DP:
            printf ("[%d] MFlops/s: %f \n",
                    cpu_id,1.0E-06*(float)(thread->pc[0]*2+thread->pc[1])/time);
            break;

        case FLOPS_SP:
            printf ("[%d] MFlops/s: %f \n",
                    cpu_id,1.0E-06*(float)(thread->pc[0]*4+thread->pc[1])/time);
            break;

        case L1:
            printf ("[%d] Guess: %f \n",cpu_id,1.0E-06*(float)((thread->pc[1]-thread->pc[0])*16)/time);
            break;

        case L2:
            printf ("[%d] L2 Load MBytes/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[0]*64)/time);
            printf ("[%d] L2 Evict MBytes/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[1]*64)/time);
            printf ("[%d] L2 bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[0]+thread->pc[1])*64)/time);
            break;

        case MEM:
            printf ("[%d] Memory bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)(thread->pc[0]*64)/time);
            break;

        case DATA:
            printf ("[%d] Store to Load ratio: 1:%f \n",cpu_id,(float)thread->pc[0]/(float)thread->pc[1]);
            break;

        case BRANCH:
            printf ("[%d] Mispredicted Branches: %f  \n",cpu_id,(float) (thread->pc[1]/(float)thread->pc[0]) * 100);
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

}



