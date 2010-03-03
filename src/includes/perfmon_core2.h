#include <stdlib.h>
#include <stdio.h>

#include <bstrlib.h>
#include <types.h>
#include <registers.h>


int
perfmon_getIndex_core2 (bstring str, PerfmonCounterIndex* index)
{
   if (biseqcstr(str,"PMC0"))
   {
       *index = PMC0;
   }
   else if (biseqcstr(str,"PMC1"))
   {
       *index = PMC1;
   }
   else
   {
       return FALSE;
   }

   return TRUE;
}


void 
perfmon_init_core2(PerfmonThread *thread)
{
    uint64_t flags = 0x0ULL;
    int cpu_id = thread->cpu_id;

    thread->counters[PMC0].config_reg = MSR_PERFEVTSEL0;
    thread->counters[PMC0].counter_reg = MSR_PMC0;
    thread->counters[PMC1].config_reg = MSR_PERFEVTSEL1;
    thread->counters[PMC1].counter_reg = MSR_PMC1;

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
perfmon_setupReport_core2(MultiplexCollections* collections)
{
    collections->numberOfCollections = 6;
    collections->collections = (PerfmonEventSet*) malloc(collections->numberOfCollections * sizeof(PerfmonEventSet));

    /*  Load/Store */
    collections->collections[0].numberOfEvents = 2;
    collections->collections[0].events = (PerfmonEventSetEntry*) malloc(2 * sizeof(PerfmonEventSetEntry));
    collections->collections[0].events[0].eventName = bfromcstr("INST_RETIRED_LOADS");
    collections->collections[0].events[0].reg = bfromcstr("PMC0");
    collections->collections[0].events[1].eventName = bfromcstr("INST_RETIRED_STORES");
    collections->collections[0].events[1].reg = bfromcstr("PMC1");

    /*  Branches */
    collections->collections[1].numberOfEvents = 2;
    collections->collections[1].events = (PerfmonEventSetEntry*) malloc(2 * sizeof(PerfmonEventSetEntry));
    collections->collections[1].events[0].eventName = bfromcstr("BR_INST_RETIRED_ANY");
    collections->collections[1].events[0].reg = bfromcstr("PMC0");
    collections->collections[1].events[1].eventName = bfromcstr("BR_INST_RETIRED_MISPRED");
    collections->collections[1].events[1].reg = bfromcstr("PMC1");

    /*  SIMD Double */
    collections->collections[2].numberOfEvents = 2;
    collections->collections[2].events = (PerfmonEventSetEntry*) malloc(2 * sizeof(PerfmonEventSetEntry));
    collections->collections[2].events[0].eventName = bfromcstr("SIMD_INST_RETIRED_PACKED_DOUBLE");
    collections->collections[2].events[0].reg = bfromcstr("PMC0");
    collections->collections[2].events[1].eventName = bfromcstr("SIMD_INST_RETIRED_SCALAR_DOUBLE");
    collections->collections[2].events[1].reg = bfromcstr("PMC1");

    /*  L1 Utilization */
    collections->collections[3].numberOfEvents = 2;
    collections->collections[3].events = (PerfmonEventSetEntry*) malloc(2 * sizeof(PerfmonEventSetEntry));
    collections->collections[3].events[0].eventName = bfromcstr("MEM_LOAD_RETIRED_L1D_LINE_MISS");
    collections->collections[3].events[0].reg = bfromcstr("PMC0");
    collections->collections[3].events[1].eventName = bfromcstr("L1D_ALL_REF");
    collections->collections[3].events[1].reg = bfromcstr("PMC1");

    /*  Memory transfers/TLB misses */
    collections->collections[4].numberOfEvents = 2;
    collections->collections[4].events = (PerfmonEventSetEntry*) malloc(2 * sizeof(PerfmonEventSetEntry));
    collections->collections[4].events[0].eventName = bfromcstr("BUS_TRANS_MEM_THIS_CORE_THIS_A");
    collections->collections[4].events[0].reg = bfromcstr("PMC0");
    collections->collections[4].events[1].eventName = bfromcstr("DTLB_MISSES_ANY");
    collections->collections[4].events[1].reg = bfromcstr("PMC1");

    /*  L2 bandwidth */
    collections->collections[5].numberOfEvents = 2;
    collections->collections[5].events = (PerfmonEventSetEntry*) malloc(2 * sizeof(PerfmonEventSetEntry));
    collections->collections[5].events[0].eventName = bfromcstr("L1D_REPL");
    collections->collections[5].events[0].reg = bfromcstr("PMC0");
    collections->collections[5].events[1].eventName = bfromcstr("L1D_M_EVICT");
    collections->collections[5].events[1].reg = bfromcstr("PMC1");
}

void
perfmon_printReport_core2(MultiplexCollections* collections)
{
    printf(HLINE);
    printf("PERFORMANCE REPORT\n");
    printf(HLINE);
    printf("\nRuntime  %.2f s\n\n",collections->time);
    /* Section 1 */
    printf("Code characteristics:\n");
    printf("\tLoad to store ratio %f \n",collections->collections[0].events[0].results[0]/collections->collections[0].events[1].results[0]);
    printf("\tPercentage SIMD vectorized double %.2f \n",(collections->collections[2].events[0].results[0]*100.0)/
            (collections->collections[2].events[0].results[0]+collections->collections[2].events[1].results[0]));
    printf("\tPercentage mispredicted branches  %.2f \n",(collections->collections[1].events[1].results[0]*100.0)/
            collections->collections[1].events[0].results[0]);
    /* Section 2 */
    printf("\nCode intensity:\n");
    printf("\tDouble precision Flops/s  %.2f MFlops/s\n",1.0E-06*(collections->collections[2].events[0].results[0]*2.0+
                   collections->collections[2].events[1].results[0] )/(double) (collections->time*0.16666666));
    printf("\nResource Utilization:\n");
    printf("\tL1 Ref per miss %.2f \n",(collections->collections[3].events[1].results[0]/collections->collections[3].events[0].results[0]));
    printf("\tRefs per TLB miss  %.2f \n", (collections->collections[3].events[1].results[0]/collections->collections[4].events[1].results[0]));
    /* Section 3 */
    printf("\nBandwidths:\n");
    printf("\tL2 bandwidth  %.2f MBytes/s\n",1.0E-06*((collections->collections[5].events[1].results[0]+
                collections->collections[5].events[0].results[0])*64.0)/(double) (collections->time*0.16666666));
    printf("\tMemory bandwidth  %.2f MBytes/s\n",1.0E-06*((collections->collections[4].events[0].results[0])*64.0)/(double) (collections->time*0.16666666));
    printf(HLINE);
}

void
perfmon_printGroups_core2(void)
{
    printf("Performance Groups: Core 2\n\n");
    printf("FLOPS_DP: Double Precision MFlops/s\n");
    printf("FLOPS_SP: Single Precision MFlops/s\n");
    printf("L2: L2 cache bandwidth in MBytes/s\n");
    printf("MEM: Main memory bandwidth in MBytes/s\n");
    printf("DATA: Load to store ratio\n");
    printf("BRANCH: Branch prediction miss rate\n");
    printf("TLB: Translation lookaside buffer miss rate\n");
    printf("CPI: cycles per instruction\n\n");
}

PerfmonGroup
perfmon_getGroupId_core2(bstring groupStr)
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
perfmon_setupCounterThread_core2(int thread_id,
        uint32_t event, uint32_t umask,
        PerfmonCounterIndex index)
{
    uint64_t flags;
    uint64_t reg = threadData[thread_id].counters[index].config_reg;
    int cpu_id = threadData[thread_id].cpu_id;
    threadData[thread_id].counters[index].init = TRUE;

    flags = msr_read(cpu_id,reg);
    flags &= ~(0xFFFFU); 

    /* Intel with standard 8 bit event mask: [7:0] */
    flags |= (umask<<8) + event;

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

    for (i=0;i<NUM_PMC;i++) {
        if (threadData[thread_id].counters[i].init == TRUE) {
            msr_write(cpu_id, threadData[thread_id].counters[i].counter_reg , 0x0ULL);
            flags |= (1<<i);  /* enable counter */
        }
    }

    if (perfmon_verbose)
    {
        printf("perfmon_start_counters: Write Register 0x%X , Flags: 0x%llX \n",MSR_PERF_GLOBAL_CTRL, LLU_CAST flags);
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

    msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);

    for (i=0;i<NUM_PMC;i++) 
    {
        if (threadData[thread_id].counters[i].init == TRUE) 
        {
            threadData[thread_id].pc[i] = msr_read(cpu_id, threadData[thread_id].counters[i].counter_reg);
        }
    }

    threadData[thread_id].cycles = msr_read(cpu_id, MSR_PERF_FIXED_CTR1);
    threadData[thread_id].instructionsRetired = msr_read(cpu_id, MSR_PERF_FIXED_CTR0);

    flags = msr_read(cpu_id,MSR_PERF_GLOBAL_STATUS);
 /*   printf ("Status: 0x%llX \n", LLU_CAST flags);*/
    if((flags & 0x3) || (flags & (0x3ULL<<32)) ) 
    {
        printf ("Overflow occured \n");
    }
}




void
perfmon_setupGroupThread_core2(int thread_id, PerfmonGroup group)
{
    bstring event_0 = bformat("NOINIT");
    bstring event_1 = bformat("NOINIT");

    switch ( group ) 
    {
        case FLOPS_DP:
            bassigncstr(event_0, "SIMD_COMP_INST_RETIRED_PACKED_DOUBLE");
            bassigncstr(event_1, "SIMD_COMP_INST_RETIRED_SCALAR_DOUBLE");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            break;

        case FLOPS_SP:
            bassigncstr(event_0, "SIMD_COMP_INST_RETIRED_PACKED_SINGLE");
            bassigncstr(event_1, "SIMD_COMP_INST_RETIRED_SCALAR_SINGLE");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            break;

        case L2:
            bassigncstr(event_0, "L1D_REPL");
            bassigncstr(event_1, "L1D_M_EVICT");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            break;

        case MEM:
            bassigncstr(event_0, "BUS_TRANS_MEM_THIS_CORE_THIS_A");
            setupCounterThread(thread_id, PMC0, event_0);
            break;

        case DATA:
            bassigncstr(event_0, "INST_RETIRED_LOADS");
            bassigncstr(event_1, "INST_RETIRED_STORES");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            break;

        case BRANCH:
            bassigncstr(event_0, "BR_INST_RETIRED_ANY");
            bassigncstr(event_1, "BR_INST_RETIRED_MISPRED");
            break;

        case CPI:
            bassigncstr(event_0, "UOPS_RETIRED_ANY");
            setupCounterThread(thread_id, PMC0, event_0);
            break;

        case TLB:
            bassigncstr(event_0, "DTLB_MISSES_ANY");
            bassigncstr(event_1, "DTLB_MISSES_MISS_LD");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            break;

        case FRONTEND:
            bassigncstr(event_0, "UOPS_RETIRED_ANY");
            bassigncstr(event_1, "DTLB_MISSES_MISS_LD");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            break;

        default:
            printf ("WARNING: Unknown Performance group %d \n",group);
            break;
    }

    bdestroy(event_0);
    bdestroy(event_1);
}

void
perfmon_printResults_core2(PerfmonThread *thread, PerfmonGroup group, float time)
{
    int cpu_id = thread->cpu_id;

    if (thread->instructionsRetired < 1.0E-06)
    {
        printf ("[%d] Cycles per uop/s: %f \n",cpu_id,0.0);
    }
    else
    {
        printf ("[%d] Cycles per uop/s: %f \n",cpu_id,(float)thread->cycles/(float)thread->instructionsRetired);
    }

    switch ( group ) 
    {
        case FLOPS_DP:
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
}




