#include <stdlib.h>
#include <stdio.h>

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
    /* UNCORE FIXED 0: Uncore cyclesa */
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

    /* Enable fixed counters */
    flags  = 0x300000000ULL;
    uflags = 0x1000000FFULL;

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
        printf("perfmon_start_counters: Write Register 0x%X , Flags: 0x%llX \n",MSR_PERF_GLOBAL_CTRL,flags);
        printf("perfmon_start_counters: Write Register 0x%X , Flags: 0x%llX \n",MSR_UNCORE_PERF_GLOBAL_CTRL,uflags);
    }

    msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, flags);
    msr_write(cpu_id, MSR_UNCORE_PERF_GLOBAL_CTRL, uflags);
    msr_write(cpu_id, MSR_PERF_GLOBAL_OVF_CTRL, 0x30000000FULL);

}


PerfmonGroup
perfmon_getGroupId_nehalem (char* groupStr)
{
	PerfmonGroup group;

	if (!strcmp("FLOPS_DP",groupStr)) 
	{
		group = FLOPS_DP;
	}
	else if (!strcmp("FLOPS_SP",groupStr)) 
	{
		group = FLOPS_SP;
	}
	else if (!strcmp("L2",groupStr)) 
	{
		group = L2;
	}
	else if (!strcmp("MEM",groupStr)) 
	{
		group = MEM;
	}
	else if (!strcmp("DATA",groupStr)) 
	{
		group = DATA;
	}
	else if (!strcmp("BRANCH",groupStr)) 
	{
		group = BRANCH;
	}
	else if (!strcmp("TLB",groupStr)) 
	{
		group = TLB;
	}
	else if (!strcmp("CPI",groupStr)) 
	{
		group = CPI;
	}
	else if (!strcmp("CLUSTER",groupStr)) 
	{
		group = CLUSTER;
	}
	else if (!strcmp("CLUSTER_FLOPS",groupStr)) 
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

    switch ( group ) {
        case FLOPS_DP:
            setupCounterThread(thread_id, PMC0, "FP_COMP_OPS_EXE_SSE_FP_PACKED");
            setupCounterThread(thread_id, PMC1, "FP_COMP_OPS_EXE_SSE_FP_SCALAR");
            setupCounterThread(thread_id, PMC2, "FP_COMP_OPS_EXE_SSE_SINGLE_PRECISION");
            setupCounterThread(thread_id, PMC3, "FP_COMP_OPS_EXE_SSE_DOUBLE_PRECISION");
            break;

        case FLOPS_SP:
            setupCounterThread(thread_id, PMC0, "FP_COMP_OPS_EXE_SSE_FP_PACKED");
            setupCounterThread(thread_id, PMC1, "FP_COMP_OPS_EXE_SSE_FP_SCALAR");
            setupCounterThread(thread_id, PMC2, "FP_COMP_OPS_EXE_SSE_SINGLE_PRECISION");
            setupCounterThread(thread_id, PMC3, "FP_COMP_OPS_EXE_SSE_DOUBLE_PRECISION");
            break;

        case L2:
            setupCounterThread(thread_id, PMC0, "L1D_REPL");
            setupCounterThread(thread_id, PMC1, "L1D_M_EVICT");
            break;

        case L3:
            setupCounterThread(thread_id, PMC0, "L2_LINES_IN_ANY");
            setupCounterThread(thread_id, PMC1, "L2_LINES_OUT_ANY");
            break;

        case MEM:
            setupCounterThread(thread_id, PMCU0, "UNC_L3_LINES_IN_ANY");
            setupCounterThread(thread_id, PMCU1, "UNC_L3_LINES_OUT_ANY");
            setupCounterThread(thread_id, PMCU2, "UNC_L3_MISS_READ");
            setupCounterThread(thread_id, PMCU3, "UNC_GQ_DATA_FROM_L3");
            setupCounterThread(thread_id, PMC0, "L2_LINES_IN_ANY");
            break;

        case DATA:
            setupCounterThread(thread_id, PMC0, "MEM_INST_RETIRED_LOADS");
            setupCounterThread(thread_id, PMC1, "MEM_INST_RETIRED_STORES");
            break;

        case BRANCH:
            setupCounterThread(thread_id, PMC0, "BR_INST_RETIRED_ALL_BRANCHES");
            setupCounterThread(thread_id, PMC1, "BR_MISP_RETIRED_ALL_BRANCHES");
            break;

        case CPI:
            setupCounterThread(thread_id, PMC0, "UOPS_RETIRED_ANY");
            break;

        case TLB:
            setupCounterThread(thread_id, PMC0, "DTLB_MISSES_ANY");
            setupCounterThread(thread_id, PMC1, "DTLB_MISSES_MISS_LD");
            break;

        case CLUSTER:
            setupCounterThread(thread_id, PMC0, "FP_COMP_OPS_EXE_X87");
            setupCounterThread(thread_id, PMC1, "FP_COMP_OPS_EXE_SSE_FP");
            setupCounterThread(thread_id, PMC2, "L2_RQSTS_MISS");
            break;

        case CLUSTER_FLOPS:
            setupCounterThread(thread_id, PMC0, "FP_COMP_OPS_EXE_SSE_FP_PACKED");
            setupCounterThread(thread_id, PMC1, "FP_COMP_OPS_EXE_SSE_FP_SCALAR");
            setupCounterThread(thread_id, PMC2, "FP_COMP_OPS_EXE_SSE_SINGLE_PRECISION");
            setupCounterThread(thread_id, PMC3, "FP_COMP_OPS_EXE_SSE_DOUBLE_PRECISION");
            break;

        default:
            break;
    }

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
                printf ("[%d] Packed MUOPS/s: %f \n",cpu_id,0.0);
                printf ("[%d] Scalar MUOPS/s: %f \n",cpu_id,0.0);
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
                printf ("[%d] Single Precision MFlops/s (SP assumed): %f \n",
                        cpu_id,0.0);
                printf ("[%d] Packed MUOPS/s: %f \n",cpu_id,0.0);
                printf ("[%d] Scalar MUOPS/s: %f \n",cpu_id,0.0);
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
                printf ("[%d] L2 Bandwidth MBytes/s: %f \n",cpu_id,0.0);
                printf ("[%d] L2 Load Bandwidth MBytes/s: %f \n",0.0);
                printf ("[%d] L2 Evict Bandwidth MBytes/s: %f \n",0.0);
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
                printf ("[%d] L3 Bandwidth MBytes/s: %f \n",cpu_id,0.0);
                printf ("[%d] L3 Load Bandwidth MBytes/s: %f \n",0.0);
                printf ("[%d] L3 Evict Bandwidth MBytes/s: %f \n",0.0);
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
                printf ("[%d] Memory bandwidth MBytes/s: %f \n",cpu_id,0.0);
                printf ("[%d] L2 Load Bandwidth MBytes/s: %f \n",cpu_id,0.0);
                printf ("[%d] L2 Evict Bandwidth MBytes/s: %f \n",cpu_id,0.0);
            }
            else
            {
                printf ("[%d] Memory bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[0]+thread->pc[1])*64)/time);
                printf ("[%d] L2 Load Bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[0])*64)/time);
                printf ("[%d] L2 Evict Bandwidth MBytes/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[1])*64)/time);
            }
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

        case CLUSTER:
            if (time < 1.0E-12)
            {
                printf ("[%d] X87 Mops/s: %f \n",cpu_id,0.0);
                printf ("[%d] SSE Mops/s: %f \n",cpu_id,0.0);
                printf ("[%d] L2 MMiss/s: %f \n",cpu_id,0.0);
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
                printf ("[%d] Packed MUOPS/s: %f \n",cpu_id,0.0);
                printf ("[%d] Scalar MUOPS/s: %f \n",cpu_id,0.0);
                printf ("[%d] SP MUOPS/s: %f \n",cpu_id,0.0);
                printf ("[%d] DP MUOPS/s: %f \n",cpu_id,0.0);
            }
            else
            {
                printf ("[%d] X87 Mops/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[0]))/time);
                printf ("[%d] SSE Mops/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[1]))/time);
                printf ("[%d] L2 MMiss/s: %f \n",cpu_id,1.0E-06*(float)((thread->pc[2]))/time);
            }
            break;

        default:
            printf ("WARNING: Unknown Performance group %d \n",group);
            break;
    }

}



