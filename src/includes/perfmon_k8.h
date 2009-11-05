#include <stdlib.h>
#include <stdio.h>

#include <types.h>
#include <registers.h>



void
perfmon_printGroups_k8 (void)
{
    printf("Performance Groups: K8\n\n");
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

PerfmonGroup
perfmon_getGroupId_k8 (char* groupStr)
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
	else
	{
		return NOGROUP;
	}

	return group;
}


void
perfmon_setupGroupThread_k8(int thread_id,PerfmonGroup group)
{

    switch ( group ) {
        case FLOPS_DP:
            setupCounterThread(thread_id, PMC0, "SSE_RETIRED_ADD_DOUBLE_FLOPS");
            setupCounterThread(thread_id, PMC1, "SSE_RETIRED_MULT_DOUBLE_FLOPS");
            setupCounterThread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
            break;

        case FLOPS_SP:
            setupCounterThread(thread_id, PMC0, "SSE_RETIRED_ADD_SINGLE_FLOPS");
            setupCounterThread(thread_id, PMC1, "SSE_RETIRED_MULT_SINGLE_FLOPS");
            setupCounterThread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
            break;

        case L1:
            setupCounterThread(thread_id, PMC0, "DATA_CACHE_REFILLS_L2_ALL");
            setupCounterThread(thread_id, PMC1, "DATA_CACHE_EVICTED_ALL");
            setupCounterThread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
            setupCounterThread(thread_id, PMC3, "DATA_CACHE_REFILLS_NORTHBRIDGE_ALL");
            break;

        case L2:
            setupCounterThread(thread_id, PMC0, "DATA_CACHE_REFILLS_L2_ALL");
            setupCounterThread(thread_id, PMC1, "DATA_CACHE_EVICTED_ALL");
            setupCounterThread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
            break;

        case L3:
            setupCounterThread(thread_id, PMC0, "L3_FILLS_ALL_ALL_CORES");
            setupCounterThread(thread_id, PMC1, "L3_READ_REQUEST_ALL_ALL_CORES");
            setupCounterThread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
            break;

        case MEM:
            break;

        case DATA:
            setupCounterThread(thread_id, PMC0, "INST_RETIRED_LOADS");
            setupCounterThread(thread_id, PMC1, "INST_RETIRED_STORES");
            setupCounterThread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
            break;

        case BRANCH:
            setupCounterThread(thread_id, PMC0, "BRANCH_RETIRED");
            setupCounterThread(thread_id, PMC1, "BRANCH_MISPREDICT_RETIRED");
            setupCounterThread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
            break;

        case CPI:
            setupCounterThread(thread_id, PMC0, "UOPS_RETIRED");
            setupCounterThread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
            break;

        case TLB:
            setupCounterThread(thread_id, PMC0, "DTLB_L2_MISS_4K");
            setupCounterThread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
            break;

        default:
            break;
    }

}


void perfmon_printResults_k8(PerfmonThread *thread, PerfmonGroup group_set, float time)
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

