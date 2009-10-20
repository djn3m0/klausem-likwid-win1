/*
 * ===========================================================================
 *
 *       Filename:  perfmon.c
 *
 *    Description:  Implementation of perfmon Module.
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
 * ===========================================================================
 */


/* #####   HEADER FILE INCLUDES   ######################################### */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <types.h>
#include <timer.h>
#include <msr.h>
#include <cpuid.h>
#include <perfmon.h>
#include <registers.h>
#include <perfmon_pm_events.h>
#include <perfmon_core2_events.h>
#include <perfmon_nehalem_events.h>
#include <perfmon_k10_events.h>
#include <perfmon_pm.h>
#include <perfmon_core2.h>
#include <perfmon_nehalem.h>
#include <perfmon_k10.h>


/* #####   EXPORTED VARIABLES   ########################################### */

int num_arch_events;
int perfmon_verbose=0;
void (*perfmon_printAvailableGroups) (void);

/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ###################### */

static PerfmonGroup groupSet = NOGROUP;
static const PerfmonHashEntry* eventHash;
static PerfmonThread summary;
static PerfmonThread* threadData;
static CyclesData timeData;
static int numThreads;

static PerfmonGroup (*getGroupIdFuncPtr) (char*);
static void (*setupGroupThreadFuncPtr) (int thread_id, PerfmonGroup group);

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ########### */

static int
getEvent(char* event_str, uint32_t* event, uint32_t* umask)
{
    int i;
    int found = 0;

    for (i=0; i< num_arch_events; i++)
    {
        if (strcmp(event_str, eventHash[i].key) == 0)
        {
            *event = eventHash[i].event.event_id;
            *umask = eventHash[i].event.umask;
            found = 1;
            break;
        }
    }

    if (found) 
    {
        if (perfmon_verbose)
        {
            printf ("Found event %s : Event_id 0x%02X Umask 0x%02X \n",
                    event_str, *event, *umask);
        }
        return TRUE;
    }
    else 
    {
        return FALSE;
    }
}

static void
initThread(int thread_id, int cpu_id)
{
    int i;

    for (i=0; i<NUM_PMC; i++)
    {
        threadData[thread_id].counters[i].label =
            (char*) malloc(500*sizeof(char));

        threadData[thread_id].counters[i].init = FALSE;
    }

    threadData[thread_id].cpu_id = cpu_id;

    switch ( cpuid_info.family ) 
    {
        case P6_FAMILY:

            switch ( cpuid_info.model ) 
            {
                case XEON_MP:

                case CORE2_65:

                case CORE2_45:

                    perfmon_init_core2(&threadData[thread_id]);
                    break;

                case NEHALEM:

                    perfmon_init_nehalem(&threadData[thread_id]);
                    break;

                default:
                    fprintf(stderr, "Unsupported Processor!\n");
                    exit(EXIT_FAILURE);
                    break;
            }
            break;

        case NETBURST_FAMILY:
            break;

        case K10_FAMILY:
            switch ( cpuid_info.model )
            {
                case BARCELONA:

                case SHANGHAI:

                case ISTANBUL:
                    perfmon_init_k10(&threadData[thread_id]);
                    break;

                default:
                    fprintf(stderr, "Unsupported Processor!\n");
                    exit(EXIT_FAILURE);
                    break;
            }

            break;

        default:
            fprintf(stderr, "Unsupported Processor!\n");
            exit(EXIT_FAILURE);
            break;
    }
}

static int
setupCounterThread(int thread_id,
        PerfmonCounterIndex index,
        char* event_str)
{
    uint64_t flags;
    uint32_t umask, event;
    uint64_t reg = threadData[thread_id].counters[index].config_reg;
    int cpu_id = threadData[thread_id].cpu_id;

    if (!getEvent(event_str, &event, &umask))
    {
        printf("ERROR: Event %s not found for current architecture\n",event_str);
        return FALSE;
    }

    strcpy(threadData[thread_id].counters[index].label, event_str);
    threadData[thread_id].counters[index].init = TRUE;

    flags = msr_read(cpu_id,reg);
    flags &= ~(0xFFFFU); 

    if (cpuid_info.family == P6_FAMILY)
    {
        msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);
        /* Intel with standard 8 bit event mask: [7:0] */
        flags |= (umask<<8) + event;
    }
    else 
    {
        /* AMD uses a 12 bit Event mask: [35:32][7:0] */
        flags |= ((uint64_t)(event>>8)<<32) + (umask<<8) + (event & ~(0xF00U));
    }

    if (perfmon_verbose)
    {
        printf("[%d] perfmon_setup_counter: Write Register 0x%llX , Flags: 0x%llX \n",cpu_id,reg,flags);
    }
    msr_write(cpu_id, reg , flags);

    return TRUE;
}

static void
startCountersThread(int thread_id)
{
    int i;
    uint64_t flags, uflags;
    int cpu_id = threadData[thread_id].cpu_id;

    switch ( cpuid_info.family ) 
    {
        case P6_FAMILY:

            switch ( cpuid_info.model ) 
            {
                case PENTIUM_M_BANIAS:

                case PENTIUM_M_DOTHAN:

                    break;

                case XEON_MP:

                case CORE2_65:

                case CORE2_45:

                case NEHALEM:

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

                    break;

                default:
                    fprintf(stderr, "Unsupported Processor!\n");
                    exit(EXIT_FAILURE);
                    break;
            }
            break;

            if (perfmon_verbose)
            {
                printf("perfmon_start_counters: Write Register 0x%X , Flags: 0x%llX \n",MSR_PERF_GLOBAL_CTRL,flags);
            }


            switch ( cpuid_info.model ) 
            {
                case PENTIUM_M_BANIAS:

                case PENTIUM_M_DOTHAN:
                    break;

                case XEON_MP:

                case CORE2_65:

                case CORE2_45:

                    msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, flags);
                    msr_write(cpu_id, MSR_PERF_GLOBAL_OVF_CTRL, 0x300000003ULL);
                    break;

                case NEHALEM:

                    if (perfmon_verbose) 
                    {
                        printf("perfmon_start_counters: Write Register 0x%X , Flags: 0x%llX \n",MSR_UNCORE_PERF_GLOBAL_CTRL,uflags);
                    }

                    msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, flags);
                    msr_write(cpu_id, MSR_UNCORE_PERF_GLOBAL_CTRL, uflags);
                    msr_write(cpu_id, MSR_PERF_GLOBAL_OVF_CTRL, 0x30000000FULL);
                    break;

                default:
                    fprintf(stderr, "Unsupported Processor!\n");
                    exit(EXIT_FAILURE);
                    break;
            }
            break;

        case NETBURST_FAMILY:
            fprintf(stderr, "Unsupported Processor!\n");
            exit(EXIT_FAILURE);
            break;

        case K10_FAMILY:
            switch ( cpuid_info.model ) 
            {
                case BARCELONA:

                case SHANGHAI:

                case ISTANBUL:

                    for (i=0;i<NUM_PMC;i++) 
                    {
                        if (threadData[thread_id].counters[i].init == TRUE) 
                        {
                            msr_write(cpu_id, threadData[thread_id].counters[i].counter_reg , 0x0ULL);
                            flags = msr_read(cpu_id,threadData[thread_id].counters[i].config_reg);
                            flags |= (1<<22);  /* enable flag */
                            if (perfmon_verbose) 
                            {
                                printf("perfmon_start_counters: Write Register 0x%llX , Flags: 0x%llX \n",threadData[thread_id].counters[i].config_reg,flags);
                            }

                            msr_write(cpu_id, threadData[thread_id].counters[i].config_reg , flags);
                        }
                    }
                    break;

                default:
                    fprintf(stderr, "Unsupported Processor!\n");
                    exit(EXIT_FAILURE);
                    break;
            }

        default:
            fprintf(stderr, "Unsupported Processor!\n");
            exit(EXIT_FAILURE);
            break;
    }
}


static void 
stopCountersThread(int thread_id)
{
    uint64_t flags;
    uint64_t uncore_cycles;
    int i;
    int cpu_id = threadData[thread_id].cpu_id;

    switch ( cpuid_info.family ) 
    {
        case P6_FAMILY:

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

            flags = msr_read(cpu_id,MSR_PERF_GLOBAL_STATUS);
            printf ("Status: 0x%llX \n",flags);
            if((flags & 0x3) || (flags & (0x3ULL<<32)) ) 
            {
                printf ("Overflow occured \n");
            }

        case NETBURST_FAMILY:
            break;

        case K10_FAMILY:

            for (i=0;i<NUM_PMC;i++) 
            {
                if (threadData[thread_id].counters[i].init == TRUE) 
                {
                    flags = msr_read(cpu_id,threadData[thread_id].counters[i].config_reg);
                    flags &= ~(1<<22);  /* clear enable flag */
                    msr_write(cpu_id, threadData[thread_id].counters[i].config_reg , flags);
                    if (perfmon_verbose)
                    {
                        printf("perfmon_stop_counters: Write Register 0x%llX , Flags: 0x%llX \n",threadData[thread_id].counters[i].config_reg,flags);
                    }
                    threadData[thread_id].pc[i] = msr_read(cpu_id, threadData[thread_id].counters[i].counter_reg);
                }
            }
            threadData[thread_id].cycles = threadData[thread_id].pc[2];

            break;

        default:
            fprintf(stderr, "Unsupported Processor!\n");
            exit(EXIT_FAILURE);
            break;
    }

}


/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ################## */


void 
perfmon_printResults()
{
    int thread_id;
    float time;
    int i;

    for (i=0;i<NUM_PMC;i++) 
    {
        summary.counters[i].label = (char*) malloc(500*sizeof(char));
        summary.pc[i] = 0;
        summary.counters[i].init = FALSE;
    }
    summary.cpu_id = 100;


    for (thread_id=0;thread_id<numThreads;thread_id++) 
    {

        printf(HLINE);
        printf ("[%d] Cycles unhalted core: %llu \n",
                threadData[thread_id].cpu_id,
                threadData[thread_id].cycles);

        for (i=0;i<NUM_PMC;i++) 
        {
            if (threadData[thread_id].counters[i].init == TRUE) 
            {
                summary.pc[i] += threadData[thread_id].pc[i];
                printf ("[%d] %s: %llu \n",
                        threadData[thread_id].cpu_id,
                        threadData[thread_id].counters[i].label,
                        threadData[thread_id].pc[i]);
            }
        }
        printf(HLINE);
        printf("[%d] ==== Derived Metrics ====\n",
                threadData[thread_id].cpu_id);
        printf(HLINE);
        time = (float)threadData[thread_id].cycles/(float)cpuid_info.clock;
        printf ("[%d] Execution time: %e sec \n",
                threadData[thread_id].cpu_id,time);

        switch ( cpuid_info.family ) 
        {
            case P6_FAMILY:

                switch ( cpuid_info.model ) 
                {
                    case PENTIUM_M_BANIAS:
                        break;

                    case PENTIUM_M_DOTHAN:
                        break;

                    case CORE_DUO:
                        break;

                    case XEON_MP:

                    case CORE2_65:

                    case CORE2_45:

                        perfmon_print_results_core2(&threadData[thread_id], groupSet, time);
                        break;

                    case NEHALEM:

                        perfmon_print_results_nehalem(&threadData[thread_id], groupSet, time);
                        break;

                    default:
                        fprintf(stderr, "Unsupported Processor!\n");
                        exit(EXIT_FAILURE);
                        break;
                }
                break;

            case NETBURST_FAMILY:
                break;

            case K10_FAMILY:
                switch ( cpuid_info.model ) 
                {
                    case BARCELONA:

                    case SHANGHAI:

                    case ISTANBUL:
                        perfmon_print_results_k10(&threadData[thread_id], groupSet, time);
                        break;

                    default:
                        fprintf(stderr, "Unsupported Processor!\n");
                        exit(EXIT_FAILURE);
                        break;
                }

            default:
                fprintf(stderr, "Unsupported Processor!\n");
                exit(EXIT_FAILURE);
                break;
        }

        printf(HLINE);
    }

    if ( numThreads > 1) 
    {
        printf(HLINE);
        printf("==== SUMMARY RESULTS ====\n");
        printf(HLINE);
        printf ("[%d] RDTSC Cycles: %llu \n",summary.cpu_id,summary.cycles);
        for (i=0;i<NUM_PMC;i++) 
        {
            if (threadData[0].counters[i].init == TRUE) 
            {
                printf ("[%d] %s: %llu \n",
                        summary.cpu_id,
                        threadData[0].counters[i].label,
                        summary.pc[i]);
            }
        }

        printf(HLINE);
        printf("==== Derived Metrics ====\n");
        printf(HLINE);
        time = (float)summary.cycles/(float)cpuid_info.clock;
        printf ("[%d] Execution time: %e sec \n",summary.cpu_id,time);
        switch ( cpuid_info.family ) 
        {
            case P6_FAMILY:

                switch ( cpuid_info.model ) 
                {
                    case PENTIUM_M_BANIAS:
                        break;

                    case PENTIUM_M_DOTHAN:
                        break;

                    case CORE_DUO:
                        break;

                    case XEON_MP:

                    case CORE2_65:

                    case CORE2_45:

                        perfmon_print_results_core2(&summary, groupSet, time);
                        break;

                    case NEHALEM:

                        perfmon_print_results_nehalem(&summary, groupSet, time);
                        break;

                    default:
                        fprintf(stderr, "Unsupported Processor!\n");
                        exit(EXIT_FAILURE);
                        break;
                }
                break;

            case NETBURST_FAMILY:
                break;

            case K10_FAMILY:
                switch ( cpuid_info.model ) 
                {
                    case BARCELONA:

                    case SHANGHAI:

                    case ISTANBUL:
                        perfmon_print_results_k10(&summary, groupSet, time);
                        break;

                    default:
                        fprintf(stderr, "Unsupported Processor!\n");
                        exit(EXIT_FAILURE);
                        break;
                }

            default:
                fprintf(stderr, "Unsupported Processor!\n");
                exit(EXIT_FAILURE);
                break;
        }
        printf(HLINE);
    }
}

void
perfmon_setCycles(uint64_t cycles)
{
    summary.cycles = cycles;
}


void
perfmon_getCycles(void)
{
    FILE *file;

    file = fopen("/tmp/perfmon_cycles.txt","r");

    if (!file) 
    {
        fprintf (stderr, "Could not open file for read!\n" );
        exit (EXIT_FAILURE);
    }

    if (fscanf(file,"%llu",&summary.cycles) != 1)
    {
        fprintf(stderr, "Failed to fscanf cycles file!\n");
        exit(EXIT_FAILURE);
    }

    fclose(file);
    remove("/tmp/perfmon_cycles.txt");
}

int
perfmon_setupGroup(char* groupString)
{
    int i;
	PerfmonGroup group;

	group = getGroupIdFuncPtr(groupString);

	if (group == NOGROUP)
	{
		return 0;
	}

    for (i=0;i<numThreads;i++)
    {
        setupGroupThreadFuncPtr(i,group);
    }

	return 1;
}

int
perfmon_setupCounter(PerfmonCounterIndex index, char* event_str)
{
    int i;

    for (i=0;i<numThreads;i++)
    {
        if (!setupCounterThread(i,index,event_str)) 
        {
            fprintf (stderr, "Unknown performance event %s\n",event_str);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

void
perfmon_startAllCounters(void)
{
    int i;

    for (i=0;i<numThreads;i++)
    {
        startCountersThread(i);
    }

    timer_startCycles(&timeData);
}

void
perfmon_stopAllCounters(void)
{
    int i;

    timer_stopCycles(&timeData);
    for (i=0;i<numThreads;i++)
    {
        stopCountersThread(i);
    }

    perfmon_setCycles(timer_printCycles(&timeData));
}

void
perfmon_startCounters(int thread_id)
{
    startCountersThread(thread_id);

}

void
perfmon_stopCounters(int thread_id)
{
    stopCountersThread(thread_id);

}

void
perfmon_init(int numThreads_local, int threads[])
{
    int i;

    numThreads = numThreads_local;
    threadData = (PerfmonThread*) malloc(numThreads * sizeof(PerfmonThread));

    switch ( cpuid_info.family ) 
    {
        case P6_FAMILY:

            switch ( cpuid_info.model ) 
            {
                case PENTIUM_M_BANIAS:

                case PENTIUM_M_DOTHAN:

                    eventHash = pm_arch_events;
                    num_arch_events = NUM_ARCH_EVENTS_PM;
					getGroupIdFuncPtr = perfmon_getGroupId_pm;
					setupGroupThreadFuncPtr = perfmon_setup_group_pm;
					perfmon_printAvailableGroups = perfmon_printGroups_pm;
                    break;

                case CORE_DUO:
                    fprintf(stderr, "Unsupported Processor!\n");
                    exit(EXIT_FAILURE);
                    break;

                case XEON_MP:

                case CORE2_65:

                case CORE2_45:

                    eventHash = core2_arch_events;
                    num_arch_events = NUM_ARCH_EVENTS_CORE2;
					getGroupIdFuncPtr = perfmon_getGroupId_core2;
					setupGroupThreadFuncPtr = perfmon_setup_group_core2;
					perfmon_printAvailableGroups = perfmon_printGroups_core2;
                    break;

                case NEHALEM:

                    eventHash = nehalem_arch_events;
                    num_arch_events = NUM_ARCH_EVENTS_NEHALEM;
					getGroupIdFuncPtr = perfmon_getGroupId_nehalem;
					setupGroupThreadFuncPtr = perfmon_setup_group_nehalem;
					perfmon_printAvailableGroups = perfmon_printGroups_nehalem;
                    break;

                default:
                    fprintf(stderr, "Unsupported Processor!\n");
                    exit(EXIT_FAILURE);
                    break;
            }
            break;

        case NETBURST_FAMILY:
            fprintf(stderr, "Unsupported Processor!\n");
            exit(EXIT_FAILURE);
            break;

        case K10_FAMILY:
            switch ( cpuid_info.model ) 
            {
                case BARCELONA:

                case SHANGHAI:

                case ISTANBUL:
                    eventHash = k10_arch_events;
                    num_arch_events = NUM_ARCH_EVENTS_K10;
					getGroupIdFuncPtr = perfmon_getGroupId_k10;
					setupGroupThreadFuncPtr = perfmon_setup_group_k10;
					perfmon_printAvailableGroups = perfmon_printGroups_k10;
                    break;

                default:
                    fprintf(stderr, "Unsupported Processor!\n");
                    exit(EXIT_FAILURE);
                    break;
            }

            break;

        default:
            fprintf(stderr, "Unsupported Processor!\n");
            exit(EXIT_FAILURE);
            break;
    }


    for (i=0;i<numThreads;i++) 
    {
        initThread(i,threads[i]);
    }
}

