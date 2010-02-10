#include <stdlib.h>
#include <stdio.h>

#include <bstrlib.h>
#include <types.h>
#include <registers.h>


void perfmon_init_nehalem(PerfmonThread *thread)
{
    uint64_t flags = 0x0ULL;
    int cpu_id = thread->cpu_id;

    thread->counters[PMC0].config_reg = MSR_PERFEVTSEL0;
    thread->counters[PMC0].counter_reg = MSR_PMC0;
    thread->counters[PMC1].config_reg = MSR_PERFEVTSEL1;
    thread->counters[PMC1].counter_reg = MSR_PMC1;
    thread->counters[PMC2].config_reg = MSR_PERFEVTSEL2;
    thread->counters[PMC2].counter_reg = MSR_PMC2;
    thread->counters[PMC3].config_reg = MSR_PERFEVTSEL3;
    thread->counters[PMC3].counter_reg = MSR_PMC3;
    thread->counters[PMCU0].config_reg = MSR_UNCORE_PERFEVTSEL0;
    thread->counters[PMCU0].counter_reg = MSR_UNCORE_PMC0;
    thread->counters[PMCU1].config_reg = MSR_UNCORE_PERFEVTSEL1;
    thread->counters[PMCU1].counter_reg = MSR_UNCORE_PMC1;
    thread->counters[PMCU2].config_reg = MSR_UNCORE_PERFEVTSEL2;
    thread->counters[PMCU2].counter_reg = MSR_UNCORE_PMC2;
    thread->counters[PMCU3].config_reg = MSR_UNCORE_PERFEVTSEL3;
    thread->counters[PMCU3].counter_reg = MSR_UNCORE_PMC3;
    thread->counters[PMCU4].config_reg = MSR_UNCORE_PERFEVTSEL4;
    thread->counters[PMCU4].counter_reg = MSR_UNCORE_PMC4;
    thread->counters[PMCU5].config_reg = MSR_UNCORE_PERFEVTSEL5;
    thread->counters[PMCU5].counter_reg = MSR_UNCORE_PMC5;
    thread->counters[PMCU6].config_reg = MSR_UNCORE_PERFEVTSEL6;
    thread->counters[PMCU6].counter_reg = MSR_UNCORE_PMC6;
    thread->counters[PMCU7].config_reg = MSR_UNCORE_PERFEVTSEL7;
    thread->counters[PMCU7].counter_reg = MSR_UNCORE_PMC7;

    msr_write(cpu_id, MSR_PERF_FIXED_CTR_CTRL, 0x0ULL);
    msr_write(cpu_id, MSR_PERFEVTSEL0, 0x0ULL);
    msr_write(cpu_id, MSR_PERFEVTSEL1, 0x0ULL);
    msr_write(cpu_id, MSR_PERFEVTSEL2, 0x0ULL);
    msr_write(cpu_id, MSR_PERFEVTSEL3, 0x0ULL);
    msr_write(cpu_id, MSR_PMC0, 0x0ULL);
    msr_write(cpu_id, MSR_PMC1, 0x0ULL);
    msr_write(cpu_id, MSR_PMC2, 0x0ULL);
    msr_write(cpu_id, MSR_PMC3, 0x0ULL);
    msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);
    msr_write(cpu_id, MSR_PERF_GLOBAL_OVF_CTRL, 0x0ULL);
    msr_write(cpu_id, MSR_PEBS_ENABLE, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_FIXED_CTR_CTRL, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_FIXED_CTR0, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PERFEVTSEL0, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PERFEVTSEL1, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PERFEVTSEL2, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PERFEVTSEL3, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PERFEVTSEL4, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PERFEVTSEL5, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PERFEVTSEL6, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PERFEVTSEL7, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PMC0, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PMC1, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PMC2, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PMC3, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PMC4, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PMC5, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PMC6, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PMC7, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PERF_GLOBAL_CTRL, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PERF_GLOBAL_OVF_CTRL, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_ADDR_OPCODE_MATCH, 0x0ULL);
    msr_write(cpu_id, MSR_OFFCORE_RSP0, 0x0ULL);

    /* initialize fixed counters
     * FIXED 0: Instructions retired
     * FIXED 1: Clocks unhalted */
    msr_write(cpu_id, MSR_PERF_FIXED_CTR_CTRL, 0x22ULL);
    /* UNCORE FIXED 0: Uncore cycles */
    msr_write(cpu_id, MSR_UNCORE_FIXED_CTR_CTRL, 0x01ULL);

    /* Preinit of PERFEVSEL registers */
    flags |= (1<<22);  /* enable flag */

    msr_write(cpu_id, MSR_UNCORE_PERFEVTSEL0, flags);
    msr_write(cpu_id, MSR_UNCORE_PERFEVTSEL1, flags);
    msr_write(cpu_id, MSR_UNCORE_PERFEVTSEL2, flags);
    msr_write(cpu_id, MSR_UNCORE_PERFEVTSEL3, flags);
    msr_write(cpu_id, MSR_UNCORE_PERFEVTSEL4, flags);
    msr_write(cpu_id, MSR_UNCORE_PERFEVTSEL5, flags);
    msr_write(cpu_id, MSR_UNCORE_PERFEVTSEL6, flags);
    msr_write(cpu_id, MSR_UNCORE_PERFEVTSEL7, flags);

    flags |= (1<<16);  /* user mode flag */
#if 0
    flags |= (1<<19);  /* pin control flag */
#endif

    msr_write(cpu_id, MSR_PERFEVTSEL0, flags);
    msr_write(cpu_id, MSR_PERFEVTSEL1, flags);
    msr_write(cpu_id, MSR_PERFEVTSEL2, flags);
    msr_write(cpu_id, MSR_PERFEVTSEL3, flags);
}

void
perfmon_printGroups_nehalem (void)
{
    printf("Performance Groups: Nehalem\n\n");
    printf("FLOPS_DP: Double Precision MFlops/s\n");
    printf("FLOPS_SP: Single Precision MFlops/s\n");
    printf("L2: L2 cache bandwidth in MBytes/s\n");
    printf("MEM: Main memory bandwidth in MBytes/s\n");
    printf("DATA: Load to store ratio\n");
    printf("BRANCH: Branch prediction miss rate\n");
    printf("TLB: Translation lookaside buffer miss rate\n");
    printf("CPI: cycles per instruction\n");
    printf("CLUSTER: Cluster monitoring \n");
    printf("CLUSTER_FLOPS: Cluster monitoring flops \n\n");
}

void
perfmon_startCountersThread_nehalem(int thread_id)
{
    int i;
    uint64_t flags, uflags;
    int cpu_id = threadData[thread_id].cpu_id;

    msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);
    msr_write(cpu_id, MSR_PERF_FIXED_CTR0, 0x0ULL);
    msr_write(cpu_id, MSR_PERF_FIXED_CTR1, 0x0ULL);
    msr_write(cpu_id, MSR_UNCORE_PERF_GLOBAL_CTRL, 0x0ULL);

    /* Enable fixed counters */
    flags  = 0x300000000ULL;
    uflags = 0x100000000ULL;

    for (i=0;i<NUM_PMC;i++) {
        if (threadData[thread_id].counters[i].init == TRUE) {
            msr_write(cpu_id, threadData[thread_id].counters[i].counter_reg , 0x0ULL);
            if (i < PMCU0) {
                flags |= (1<<i);  /* enable counter */
            } else {
                uflags |= (1<<(i-PMCU0));  /* enable uncore counter */
            }
        }
    }

    if (perfmon_verbose)
    {
        printf("perfmon_start_counters: Write Register 0x%X , Flags: 0x%llX \n",MSR_PERF_GLOBAL_CTRL, LLU_CAST flags);
        printf("perfmon_start_counters: Write Register 0x%X , Flags: 0x%llX \n",MSR_UNCORE_PERF_GLOBAL_CTRL, LLU_CAST uflags);
    }

    msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, flags);
    msr_write(cpu_id, MSR_UNCORE_PERF_GLOBAL_CTRL, uflags);
    msr_write(cpu_id, MSR_PERF_GLOBAL_OVF_CTRL, 0x30000000FULL);

}
void 
perfmon_stopCountersThread_nehalem(int thread_id)
{
    uint64_t flags;
    int i;
    int cpu_id = threadData[thread_id].cpu_id;

    msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);
    if (cpuid_info.model == NEHALEM)
    {
        msr_write(cpu_id, MSR_UNCORE_PERF_GLOBAL_CTRL, 0x0ULL);
    }

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
    printf ("Status: 0x%llX \n", LLU_CAST flags);
    if((flags & 0x3) || (flags & (0x3ULL<<32)) ) 
    {
        printf ("Overflow occured \n");
    }

}




PerfmonGroup
perfmon_getGroupId_nehalem (bstring groupStr)
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
	else if (biseqcstr(groupStr,"CLUSTER")) 
	{
		group = CLUSTER;
	}
	else if (biseqcstr(groupStr,"CLUSTER_FLOPS")) 
	{
		group = CLUSTER_FLOPS;
	}
	else
	{
		return NOGROUP;
	}

	return group;
}




void perfmon_setupGroupThread_nehalem(int thread_id,PerfmonGroup group)
{
    bstring event_0 = bformat("NOINIT");
    bstring event_1 = bformat("NOINIT");
    bstring event_2 = bformat("NOINIT");
    bstring event_3 = bformat("NOINIT");

    switch ( group ) {
        case FLOPS_DP:
            bassigncstr(event_0, "FP_COMP_OPS_EXE_SSE_FP_PACKED");
            bassigncstr(event_1, "FP_COMP_OPS_EXE_SSE_FP_SCALAR");
            bassigncstr(event_2, "FP_COMP_OPS_EXE_SSE_SINGLE_PRECISION");
            bassigncstr(event_3, "FP_COMP_OPS_EXE_SSE_DOUBLE_PRECISION");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            setupCounterThread(thread_id, PMC2, event_2);
            setupCounterThread(thread_id, PMC3, event_3);
            break;

        case FLOPS_SP:
            bassigncstr(event_0, "FP_COMP_OPS_EXE_SSE_FP_PACKED");
            bassigncstr(event_1, "FP_COMP_OPS_EXE_SSE_FP_SCALAR");
            bassigncstr(event_2, "FP_COMP_OPS_EXE_SSE_SINGLE_PRECISION");
            bassigncstr(event_3, "FP_COMP_OPS_EXE_SSE_DOUBLE_PRECISION");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            setupCounterThread(thread_id, PMC2, event_2);
            setupCounterThread(thread_id, PMC3, event_3);
            break;

        case L2:
            bassigncstr(event_0, "L1D_REPL");
            bassigncstr(event_1, "L1D_M_EVICT");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            break;

        case L3:
            bassigncstr(event_0, "L2_LINES_IN_ANY");
            bassigncstr(event_1, "L2_LINES_OUT_ANY");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            break;

        case MEM:
            bassigncstr(event_0, "UNC_L3_LINES_IN_ANY");
            bassigncstr(event_1, "UNC_L3_LINES_OUT_ANY");
            bassigncstr(event_2, "UNC_L3_MISS_READ");
            bassigncstr(event_3, "UNC_GQ_DATA_FROM_L3");
            setupCounterThread(thread_id, PMCU0, event_0);
            setupCounterThread(thread_id, PMCU1, event_1);
            setupCounterThread(thread_id, PMCU2, event_2);
            setupCounterThread(thread_id, PMCU3, event_3);
            break;

        case DATA:
            bassigncstr(event_0, "MEM_INST_RETIRED_LOADS");
            bassigncstr(event_1, "MEM_INST_RETIRED_STORES");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            break;

        case BRANCH:
            bassigncstr(event_0, "BR_INST_RETIRED_ALL_BRANCHES");
            bassigncstr(event_1, "BR_MISP_RETIRED_ALL_BRANCHES");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
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

        case CLUSTER:
            bassigncstr(event_0, "FP_COMP_OPS_EXE_X87");
            bassigncstr(event_1, "FP_COMP_OPS_EXE_SSE_FP");
            bassigncstr(event_2, "L2_RQSTS_MISS");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            setupCounterThread(thread_id, PMC2, event_2);
            break;

        case CLUSTER_FLOPS:
            bassigncstr(event_0, "FP_COMP_OPS_EXE_SSE_FP_PACKED");
            bassigncstr(event_1, "FP_COMP_OPS_EXE_SSE_FP_SCALAR");
            bassigncstr(event_2, "FP_COMP_OPS_EXE_SSE_SINGLE_PRECISION");
            bassigncstr(event_3, "FP_COMP_OPS_EXE_SSE_DOUBLE_PRECISION");
            setupCounterThread(thread_id, PMC0, event_0);
            setupCounterThread(thread_id, PMC1, event_1);
            setupCounterThread(thread_id, PMC2, event_2);
            setupCounterThread(thread_id, PMC3, event_3);
            break;

        default:
            break;
    }

    bdestroy(event_0);
    bdestroy(event_1);
    bdestroy(event_2);
    bdestroy(event_3);
}


void perfmon_printResults_nehalem(PerfmonThread *thread, PerfmonGroup group, float time)
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

    switch ( group) {
        case FLOPS_DP:
            if (time < 1.0E-12)
            {
                printf ("[%d] Double Precision MFlops/s (DP assumed): %f \n",
                        cpu_id,0.0);
                printf ("[%d] Packed MUOPS/s: %f \n",cpu_id,0.0F);
                printf ("[%d] Scalar MUOPS/s: %f \n",cpu_id,0.0F);
            }
            else
            {
                printf ("[%d] Double Precision MFlops/s (DP assumed): %f \n",
                        cpu_id,1.0E-06*(float)((thread->pc[0]*2)+thread->pc[1])/time);
                printf ("[%d] Packed MUOPS/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[0]))/time);
                printf ("[%d] Scalar MUOPS/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[1]))/time);
            }
            break;

        case FLOPS_SP:
            if (time < 1.0E-12)
            {
                printf ("[%d] Single Precision MFlops/s (SP assumed): %f \n", cpu_id,0.0F);
                printf ("[%d] Packed MUOPS/s: %f \n",cpu_id,0.0F);
                printf ("[%d] Scalar MUOPS/s: %f \n",cpu_id,0.0F);
            }
            else
            {
                printf ("[%d] Single Precision MFlops/s (SP assumed): %f \n",
                        cpu_id,1.0E-06*(float)((thread->pc[0]*4)+thread->pc[1])/time);
                printf ("[%d] Packed MUOPS/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[0]))/time);
                printf ("[%d] Scalar MUOPS/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[1]))/time);
            }
            break;

        case L2:
            if (time < 1.0E-12)
            {
                printf ("[%d] L2 Bandwidth MBytes/s: %f \n",cpu_id,0.0F);
                printf ("[%d] L2 Load Bandwidth MBytes/s: %f \n",cpu_id,0.0F);
                printf ("[%d] L2 Evict Bandwidth MBytes/s: %f \n",cpu_id,0.0F);
            }
            else
            {
                printf ("[%d] L2 Bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[0]+thread->pc[1])*64)/time);
                printf ("[%d] L2 Load Bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[0])*64)/time);
                printf ("[%d] L2 Evict Bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[1])*64)/time);
            }
            break;

        case L3:
            if (time < 1.0E-12)
            {
                printf ("[%d] L3 Bandwidth MBytes/s: %f \n",cpu_id,0.0F);
                printf ("[%d] L3 Load Bandwidth MBytes/s: %f \n",cpu_id,0.0F);
                printf ("[%d] L3 Evict Bandwidth MBytes/s: %f \n",cpu_id,0.0F);
            }
            else
            {
                printf ("[%d] L3 Bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[0]+thread->pc[1])*64)/time);
                printf ("[%d] L3 Load Bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[0])*64)/time);
                printf ("[%d] L3 Evict Bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[1])*64)/time);
            }
            break;

        case MEM:
            if (time < 1.0E-12)
            {
                printf ("[%d] Memory bandwidth MBytes/s: %f \n",cpu_id,0.0F);
            }
            else
            {
                printf ("[%d] Memory bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[4]+thread->pc[5])*64)/time);
            }
            break;

        case DATA:
            printf ("[%d] Store to Load ratio: 1:%f \n",cpu_id,(float)thread->pc[0]/(float)thread->pc[1]);
            break;

        case BRANCH:
            printf ("[%d] Ratio Mispredicted Branches: %f \n",cpu_id,(float)thread->pc[1]/(float)thread->pc[0]);
            break;

        case TLB:
            break;

        case CPI:
            printf ("[%d] Cycles per uop/s: %f \n",cpu_id,(float)thread->cycles/(float)thread->pc[0]);
            break;

        case CLUSTER:
            if (time < 1.0E-12)
            {
                printf ("[%d] X87 Mops/s: %f \n",cpu_id,0.0F);
                printf ("[%d] SSE Mops/s: %f \n",cpu_id,0.0F);
                printf ("[%d] L2 MMiss/s: %f \n",cpu_id,0.0F);
            }
            else
            {
                printf ("[%d] X87 Mops/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[0]))/time);
                printf ("[%d] SSE Mops/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[1]))/time);
                printf ("[%d] L2 MMiss/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[2]))/time);
            }
            break;

        case CLUSTER_FLOPS:
            if (time < 1.0E-12)
            {
                printf ("[%d] Packed MUOPS/s: %f \n",cpu_id,0.0F);
                printf ("[%d] Scalar MUOPS/s: %f \n",cpu_id,0.0F);
                printf ("[%d] SP MUOPS/s: %f \n",cpu_id,0.0F);
                printf ("[%d] DP MUOPS/s: %f \n",cpu_id,0.0F);
            }
            else
            {
                printf ("[%d] X87 Mops/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[0]))/time);
                printf ("[%d] SSE Mops/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[1]))/time);
                printf ("[%d] L2 MMiss/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[2]))/time);
            }
            break;

        default:
            break;
    }

}



