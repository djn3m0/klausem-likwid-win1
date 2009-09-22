#include <stdlib.h>
#include <stdio.h>

#include <types.h>
#include <registers.h>


void perfmon_init_core2(PerfmonThread *thread)
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



void perfmon_setup_group_core2(int thread_id, PerfmonGroup group)
{

    switch ( group ) 
    {
        case STD:
            setupCounterThread(thread_id, PMC0, "SIMD_COMP_INST_RETIRED_PACKED_DOUBLE");
            setupCounterThread(thread_id, PMC1, "SIMD_COMP_INST_RETIRED_SCALAR_DOUBLE");
            break;

        case FLOPS_SP:
            setupCounterThread(thread_id, PMC0, "SIMD_COMP_INST_RETIRED_PACKED_SINGLE");
            setupCounterThread(thread_id, PMC1, "SIMD_COMP_INST_RETIRED_SCALAR_SINGLE");
            break;

        case L1:
            setupCounterThread(thread_id, PMC0, "SIMD_COMP_INST_RETIRED_PACKED_SINGLE");
            setupCounterThread(thread_id, PMC1, "SIMD_INST_RETIRED_PACKED_SINGLE");
            break;

        case L2:
            setupCounterThread(thread_id, PMC0, "L1D_REPL");
            setupCounterThread(thread_id, PMC1, "L1D_M_EVICT");
            break;

        case MEM:
            setupCounterThread(thread_id, PMC0, "BUS_TRANS_MEM_THIS_CORE_THIS_A");
            break;

        case DATA:
            setupCounterThread(thread_id, PMC0, "INST_RETIRED_LOADS");
            setupCounterThread(thread_id, PMC1, "INST_RETIRED_STORES");
            break;

        case BRANCH:
            setupCounterThread(thread_id, PMC0, "BR_INST_RETIRED_ANY");
            setupCounterThread(thread_id, PMC1, "BR_INST_RETIRED_MISPRED");
            break;

        case CPI:
            setupCounterThread(thread_id, PMC0, "UOPS_RETIRED_ANY");
            break;

        case TLB:
            setupCounterThread(thread_id, PMC0, "DTLB_MISSES_ANY");
            setupCounterThread(thread_id, PMC1, "DTLB_MISSES_MISS_LD");
            break;

        default:
            break;
    }

}

void perfmon_print_results_core2(PerfmonThread *thread, PerfmonGroup group_set, float time)
{
    int cpu_id = thread->cpu_id;

    switch ( group_set ) 
    {
        case STD:
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
            printf ("[%d] Ratio Mispredicted Branches: %f \n",cpu_id,(float)thread->pc[1]/(float)thread->pc[0]);
            break;

        case CPI:
            printf ("[%d] Cycles per uop/s: %f \n",cpu_id,(float)thread->cycles/(float)thread->pc[0]);
            break;

        default:
            break;
    }

}



