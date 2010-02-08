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


/* #####   HEADER FILE INCLUDES   ######################################### */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <bstrlib.h>
#include <types.h>
#include <timer.h>
#include <msr.h>
#include <cpuid.h>
#include <perfmon.h>
#include <registers.h>
#include <perfmon_pm_events.h>
#include <perfmon_core2_events.h>
#include <perfmon_nehalem_events.h>
#include <perfmon_k8_events.h>
#include <perfmon_k10_events.h>


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

/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ##################### */

static int getEvent(bstring , uint32_t* , uint32_t* );
static void initThread(int , int );
static int setupCounterThread(int , PerfmonCounterIndex , bstring );

/* These functions are all static and need access to the module variables */
#include <perfmon_pm.h>
#include <perfmon_core2.h>
#include <perfmon_nehalem.h>
#include <perfmon_k8.h>
#include <perfmon_k10.h>

/* #####   FUNCTION POINTERS  -  LOCAL TO THIS SOURCE FILE ################ */

static void (*initThreadArch) (PerfmonThread *thread);
static PerfmonGroup (*getGroupId) (bstring);
static void (*setupGroupThread) (int thread_id, PerfmonGroup group);
static void (*startCountersThread) (int thread_id);
static void (*stopCountersThread) (int thread_id);
static void (*printResults) (
        PerfmonThread *thread,
        PerfmonGroup group_set,
        float time);

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ########### */

static int
getEvent(bstring event_str, uint32_t* event, uint32_t* umask)
{
    int i;
    int found = FALSE;

    for (i=0; i< num_arch_events; i++)
    {
        if (biseqcstr(event_str, eventHash[i].key))
        {
            *event = eventHash[i].event.event_id;
            *umask = eventHash[i].event.umask;
            found = TRUE;
            break;
        }
    }

    if (perfmon_verbose)
    {
        printf ("Found event %s : Event_id 0x%02X Umask 0x%02X \n",
                event_str->data, *event, *umask);
    }

    return found;

}

static void
initThread(int thread_id, int cpu_id)
{
    int i;

    for (i=0; i<NUM_PMC; i++)
    {
        threadData[thread_id].counters[i].init = FALSE;
    }

    threadData[thread_id].cpu_id = cpu_id;
    initThreadArch(&threadData[thread_id]);

}

static int
setupCounterThread(int thread_id,
        PerfmonCounterIndex index,
        bstring event_str)
{
    uint64_t flags;
    uint32_t umask = 0, event = 0;
    uint64_t reg = threadData[thread_id].counters[index].config_reg;
    int cpu_id = threadData[thread_id].cpu_id;

    if (!getEvent(event_str, &event, &umask))
    {
        printf("ERROR: Event %s not found for current architecture\n",event_str->data);
        return FALSE;
    }

    threadData[thread_id].counters[index].label = bstrcpy(event_str);
    threadData[thread_id].counters[index].init = TRUE;

    flags = msr_read(cpu_id,reg);
    flags &= ~(0xFFFFU); 

    if (cpuid_info.family == P6_FAMILY)
    {

        if (cpuid_info.model > 0x0FU)
        {
            msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);
        }
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
        printf("[%d] perfmon_setup_counter: Write Register 0x%llX , Flags: 0x%llX \n",
               cpu_id,
               LLU_CAST reg,
               LLU_CAST flags);
    }
    msr_write(cpu_id, reg , flags);

    return TRUE;
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
        summary.pc[i] = 0;
        summary.instructionsRetired = 0;
        summary.counters[i].init = FALSE;
    }
    summary.cpu_id = 100;


    for (thread_id=0;thread_id<numThreads;thread_id++) 
    {

        printf(HLINE);
        printf ("[%d] Instruction retired any: %llu \n",
                threadData[thread_id].cpu_id,
                LLU_CAST threadData[thread_id].instructionsRetired);
        printf ("[%d] Cycles unhalted core: %llu \n",
                threadData[thread_id].cpu_id,
                LLU_CAST threadData[thread_id].cycles);
        summary.instructionsRetired += threadData[thread_id].instructionsRetired;

        for (i=0;i<NUM_PMC;i++) 
        {
            if (threadData[thread_id].counters[i].init == TRUE) 
            {
                summary.pc[i] += threadData[thread_id].pc[i];
                printf ("[%d] %s: %llu \n",
                        threadData[thread_id].cpu_id,
                        threadData[thread_id].counters[i].label->data,
                        LLU_CAST threadData[thread_id].pc[i]);
            }
        }
        printf(HLINE);
        printf("[%d] ==== Derived Metrics ====\n",
                threadData[thread_id].cpu_id);
        printf(HLINE);
        time = (float)threadData[thread_id].cycles/(float)cpuid_info.clock;
        printf ("[%d] Execution time: %e sec \n",
                threadData[thread_id].cpu_id,time);

        printResults(&threadData[thread_id], groupSet, time);
        printf(HLINE);
    }


    if ( numThreads > 1) 
    {
        printf(HLINE);
        printf("==== SUMMARY RESULTS ====\n");
        printf(HLINE);
        printf ("[%d] RDTSC Cycles: %llu \n",summary.cpu_id, LLU_CAST summary.cycles);
        printf ("[%d] Instructions retired any: %llu \n",summary.cpu_id, LLU_CAST summary.instructionsRetired);
        for (i=0;i<NUM_PMC;i++) 
        {
            if (threadData[0].counters[i].init == TRUE) 
            {
                printf ("[%d] %s: %llu \n",
                        summary.cpu_id,
                        threadData[0].counters[i].label->data,
                         LLU_CAST summary.pc[i]);
            }
        }

        printf(HLINE);
        printf("==== Derived Metrics ====\n");
        printf(HLINE);
        time = (float)summary.cycles/(float)cpuid_info.clock;
        printf ("[%d] Execution time: %e sec \n",summary.cpu_id,time);

        printResults(&summary, groupSet, time);
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

    if (fscanf(file,"%llu",(long long unsigned int *) &summary.cycles) != 1)
    {
        fprintf(stderr, "Failed to fscanf cycles file!\n");
        exit(EXIT_FAILURE);
    }

    fclose(file);
    remove("/tmp/perfmon_cycles.txt");
}

int
perfmon_setupGroup(bstring groupString)
{
    int i;

    groupSet = getGroupId(groupString);

    if (groupSet == NOGROUP)
    {
        return FALSE;
    }

    for (i=0;i<numThreads;i++)
    {
        setupGroupThread(i,groupSet);
    }

    return TRUE;
}

int
perfmon_setupCounter(PerfmonCounterIndex index, bstring event_str)
{
    int i;

    for (i=0;i<numThreads;i++)
    {
        if (!setupCounterThread(i,index,event_str)) 
        {
            fprintf (stderr, "Unknown performance event %s\n",event_str->data);
            return FALSE;
        }
    }

    return TRUE;
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

                    initThreadArch = perfmon_init_pm;
                    getGroupId = perfmon_getGroupId_pm;
                    setupGroupThread = perfmon_setupGroupThread_pm;
                    startCountersThread = perfmon_startCountersThread_pm;
                    stopCountersThread = perfmon_stopCountersThread_pm;
                    printResults = perfmon_print_results_pm;
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

                    initThreadArch = perfmon_init_core2;
                    getGroupId = perfmon_getGroupId_core2;
                    setupGroupThread = perfmon_setupGroupThread_core2;
                    startCountersThread = perfmon_startCountersThread_core2;
                    stopCountersThread = perfmon_stopCountersThread_core2;
                    printResults = perfmon_printResults_core2;
                    perfmon_printAvailableGroups = perfmon_printGroups_core2;
                    break;

                case NEHALEM_BLOOMFIELD:

                case NEHALEM_LYNNFIELD:

                    eventHash = nehalem_arch_events;
                    num_arch_events = NUM_ARCH_EVENTS_NEHALEM;

                    initThreadArch = perfmon_init_nehalem;
                    getGroupId = perfmon_getGroupId_nehalem;
                    setupGroupThread = perfmon_setupGroupThread_nehalem;
                    startCountersThread = perfmon_startCountersThread_nehalem;
                    stopCountersThread = perfmon_stopCountersThread_core2;
                    printResults = perfmon_printResults_nehalem;
                    perfmon_printAvailableGroups = perfmon_printGroups_nehalem;
                    break;

                default:
                    fprintf(stderr, "Unsupported Processor!\n");
                    exit(EXIT_FAILURE);
                    break;
            }
            break;

        case K8_FAMILY:
            eventHash = k8_arch_events;
            num_arch_events = NUM_ARCH_EVENTS_K8;

            initThreadArch = perfmon_init_k10;
            getGroupId = perfmon_getGroupId_k8;
            setupGroupThread = perfmon_setupGroupThread_k8;
            startCountersThread = perfmon_startCountersThread_k10;
            stopCountersThread = perfmon_stopCountersThread_k10;
            printResults = perfmon_printResults_k8;
            perfmon_printAvailableGroups = perfmon_printGroups_k8;
            break;


        case K10_FAMILY:
            eventHash = k10_arch_events;
            num_arch_events = NUM_ARCH_EVENTS_K10;

            initThreadArch = perfmon_init_k10;
            getGroupId = perfmon_getGroupId_k10;
            setupGroupThread = perfmon_setupGroupThread_k10;
            startCountersThread = perfmon_startCountersThread_k10;
            stopCountersThread = perfmon_stopCountersThread_k10;
            printResults = perfmon_printResults_k10;
            perfmon_printAvailableGroups = perfmon_printGroups_k10;
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
