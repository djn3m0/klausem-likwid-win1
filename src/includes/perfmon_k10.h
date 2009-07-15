#include <stdlib.h>
#include <stdio.h>

#include <types.h>
#include <registers.h>


void perfmon_init_k10(perfmon_thread_t *thread)
{
    uint64 flags = 0x0ULL;
    int cpu_id = thread->cpu_id;

    thread->counters[PMC0].config_reg = MSR_AMD_PERFEVTSEL0;
    thread->counters[PMC0].counter_reg = MSR_AMD_PMC0;
    thread->counters[PMC1].config_reg = MSR_AMD_PERFEVTSEL1;
    thread->counters[PMC1].counter_reg = MSR_AMD_PMC1;
    thread->counters[PMC2].config_reg = MSR_AMD_PERFEVTSEL2;
    thread->counters[PMC2].counter_reg = MSR_AMD_PMC2;
    thread->counters[PMC3].config_reg = MSR_AMD_PERFEVTSEL3;
    thread->counters[PMC3].counter_reg = MSR_AMD_PMC3;

    writeMSR(cpu_id, MSR_AMD_PERFEVTSEL0, 0x0ULL);
    writeMSR(cpu_id, MSR_AMD_PERFEVTSEL1, 0x0ULL);
    writeMSR(cpu_id, MSR_AMD_PERFEVTSEL2, 0x0ULL);
    writeMSR(cpu_id, MSR_AMD_PERFEVTSEL3, 0x0ULL);

    flags |= (1<<16);  /* user mode flag */

    writeMSR(cpu_id, MSR_AMD_PERFEVTSEL0, flags);
    writeMSR(cpu_id, MSR_AMD_PERFEVTSEL1, flags);
    writeMSR(cpu_id, MSR_AMD_PERFEVTSEL2, flags);
    writeMSR(cpu_id, MSR_AMD_PERFEVTSEL3, flags);


}



void perfmon_setup_group_k10(int thread_id,perfmon_group_t group)
{

    switch ( group ) {
	case STD:	
	    perfmon_setup_counter_thread(thread_id, PMC0, "SSE_RETIRED_ADD_DOUBLE_FLOPS");
	    perfmon_setup_counter_thread(thread_id, PMC1, "SSE_RETIRED_MULT_DOUBLE_FLOPS");
	    perfmon_setup_counter_thread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
	    break;

	case FLOPS_SP:	
	    perfmon_setup_counter_thread(thread_id, PMC0, "SSE_RETIRED_ADD_SINGLE_FLOPS");
	    perfmon_setup_counter_thread(thread_id, PMC1, "SSE_RETIRED_MULT_SINGLE_FLOPS");
	    perfmon_setup_counter_thread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
	    break;

	case L1:	
	    perfmon_setup_counter_thread(thread_id, PMC0, "DATA_CACHE_REFILLS_L2_ALL");
	    perfmon_setup_counter_thread(thread_id, PMC1, "DATA_CACHE_EVICTED_ALL");
	    perfmon_setup_counter_thread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
	    perfmon_setup_counter_thread(thread_id, PMC3, "DATA_CACHE_REFILLS_NORTHBRIDGE_ALL");
	    break;

	case L2:	
	    perfmon_setup_counter_thread(thread_id, PMC0, "DATA_CACHE_REFILLS_L2_ALL");
	    perfmon_setup_counter_thread(thread_id, PMC1, "DATA_CACHE_EVICTED_ALL");
	    perfmon_setup_counter_thread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
	    break;

	case L3:	
	    perfmon_setup_counter_thread(thread_id, PMC0, "L3_FILLS_ALL_ALL_CORES");
	    perfmon_setup_counter_thread(thread_id, PMC1, "L3_READ_REQUEST_ALL_ALL_CORES");
	    perfmon_setup_counter_thread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
	    break;

	case MEM:	
	    break;

	case DATA:	
	    perfmon_setup_counter_thread(thread_id, PMC0, "INST_RETIRED_LOADS");
	    perfmon_setup_counter_thread(thread_id, PMC1, "INST_RETIRED_STORES");
	    perfmon_setup_counter_thread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
	    break;

	case BRANCH:	
	    perfmon_setup_counter_thread(thread_id, PMC0, "BRANCH_RETIRED");
	    perfmon_setup_counter_thread(thread_id, PMC1, "BRANCH_MISPREDICT_RETIRED");
	    perfmon_setup_counter_thread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
	    break;

	case CPI:	
	    perfmon_setup_counter_thread(thread_id, PMC0, "UOPS_RETIRED");
	    perfmon_setup_counter_thread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
	    break;

	case TLB:	
	    perfmon_setup_counter_thread(thread_id, PMC0, "DTLB_L2_MISS_4K");
	    perfmon_setup_counter_thread(thread_id, PMC2, "CPU_CLOCKS_UNHALTED");
	    break;

	default:	
	    break;
    }

}


void perfmon_print_results_k10(perfmon_thread_t *thread, perfmon_group_t group_set, float time)
{
    int cpu_id = thread->cpu_id;

    switch ( group_set ) {
	case STD:	
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

