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
#include <asciiTable.h>
#include <registers.h>
#include <perfmon_pm_events.h>
#include <perfmon_core2_events.h>
#include <perfmon_nehalem_events.h>
#include <perfmon_k8_events.h>
#include <perfmon_k10_events.h>


/* #####   EXPORTED VARIABLES   ########################################### */

int num_arch_events;
int perfmon_verbose=0;
PerfmonThread* threadData;
int perfmon_numThreads;
int perfmon_numRegions;
LikwidResults* perfmon_results;

/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ###################### */

static PerfmonGroup groupSet = NOGROUP;
static const PerfmonHashEntry* eventHash;
static PerfmonThread summary;
static CyclesData timeData;

/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ##################### */

static void initThread(int , int );
static int setupCounterThread(int , PerfmonCounterIndex , bstring );

#include <perfmon_pm.h>
#include <perfmon_core2.h>
#include <perfmon_nehalem.h>
#include <perfmon_k8.h>
#include <perfmon_k10.h>

/* #####  EXPORTED  FUNCTION POINTERS   ################################### */
void (*perfmon_printAvailableGroups) (void);
void (*perfmon_startCountersThread) (int thread_id);
void (*perfmon_stopCountersThread) (int thread_id);
int  (*perfmon_getIndex) (bstring reg, PerfmonCounterIndex* index);
void (*perfmon_setupCounterThread) (int thread_id,
        uint32_t umask, uint32_t event, PerfmonCounterIndex index);
void (*perfmon_setupReport) (MultiplexCollections* collections);
void (*perfmon_printReport) (MultiplexCollections* collections);

/* #####   FUNCTION POINTERS  -  LOCAL TO THIS SOURCE FILE ################ */

static void (*initThreadArch) (PerfmonThread *thread);
static PerfmonGroup (*getGroupId) (bstring);
static void (*setupGroupThread) (int thread_id, PerfmonGroup group);
static void (*printResults) (
        PerfmonThread *thread,
        PerfmonGroup group_set,
        float time);

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ########### */


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

static void
setupCounter( PerfmonCounterIndex index, bstring event_str)
{
    uint64_t flags;
    uint32_t umask = 0, event = 0;
    uint64_t reg = threadData[thread_id].counters[index].config_reg;

    if (!perfmon_getEvent(event_str, &event, &umask))
    {
        fprintf (stderr,"ERROR: Event %s not found for current architecture\n",event_str->data );
        exit (EXIT_FAILURE);
    }

    threadData[thread_id].counters[index].label = bstrcpy(event_str);
    threadData[thread_id].counters[index].init = TRUE;

    flags = msr_read(threadData[0].cpu_id,reg);
    flags &= ~(0xFFFFU); 

    if (cpuid_info.family == P6_FAMILY)
    {

        if (cpuid_info.model > 0x0FU)
        {
             for (int i=0;i<perfmon_numThreads;i++)
                 msr_write(threadData[i].cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);
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
    
    for (int i=0;i<perfmon_numThreads;i++)
        msr_write(threadData[i].cpu_id, reg , flags);

    return TRUE;
}

struct cbsScan{
	/* Parse state */
	bstring src;
	int line;
};

#define CHECKERROR \
        if (ret == EOF) \
        { \
            fprintf (stderr, "sscanf: Failed to read marker file!\n" ); \
            exit (EXIT_FAILURE); \
        }


static int lineCb (void* parm, int ofs, int len)
{
    int ret;
    struct cbsScan* st = (struct cbsScan*) parm;
    struct bstrList* strList;
    bstring line;

    if (!len) return 1;

    line = blk2bstr (st->src->data + ofs, len);

    if (st->line < perfmon_numRegions)
    {
        int id;
        strList = bsplit(line,':');

        if( strList->qty < 2 )
        {
            fprintf (stderr, "bsplit 0: Failed to read marker file!\n" );
            exit (EXIT_FAILURE);
        }
        ret = sscanf (bdata(strList->entry[0]), "%d", &id); CHECKERROR;
        perfmon_results[id].tag = bstrcpy(strList->entry[1]);
        bstrListDestroy(strList);
    }
    else
    {
        int tagId;
        int threadId;

        strList = bsplit(line,32);

        if( strList->qty < (5+NUM_PMC))
        {
            fprintf (stderr, "bsplit 1: Failed to read marker file!\n" );
            exit (EXIT_FAILURE);
        }

        ret = sscanf (bdata(strList->entry[0]), "%d", &tagId); CHECKERROR;
        ret = sscanf (bdata(strList->entry[1]), "%d", &threadId); CHECKERROR;
        ret = sscanf (bdata(strList->entry[2]), "%lf", &perfmon_results[tagId].time[threadId]); CHECKERROR;
        ret = sscanf (bdata(strList->entry[3]), "%lf", &perfmon_results[tagId].cycles[threadId]); CHECKERROR;
        ret = sscanf (bdata(strList->entry[4]), "%lf", &perfmon_results[tagId].instructions[threadId]); CHECKERROR;

        for (int i=0;i<NUM_PMC; i++)
        {
            ret = sscanf (bdata(strList->entry[5+i]), "%lf", &perfmon_results[tagId].counters[threadId][i]); CHECKERROR;
        }

        bstrListDestroy(strList);
    }

    st->line++;
    bdestroy(line);
    return 1;
}

static void
readMarkerFile(char* filename)
{
	int numberOfThreads=0;
	int ret;
	int i,j,k;
    struct cbsScan sl;
	FILE * fp;

	if (NULL != (fp = fopen (filename, "r"))) 
	{
		bstring src = bread ((bNread) fread, fp);

		/* read header info */
		ret = sscanf (bdata(src), "%d %d", &numberOfThreads, &perfmon_numRegions); CHECKERROR;
		perfmon_results = (LikwidResults*) malloc(perfmon_numRegions * sizeof(LikwidResults));

		/* allocate  LikwidResults struct */
		for (i=0;i<perfmon_numRegions; i++)
		{
			perfmon_results[i].time = (double*) malloc(numberOfThreads * sizeof(double));
			perfmon_results[i].cycles = (double*) malloc(numberOfThreads * sizeof(double));
			perfmon_results[i].instructions = (double*) malloc(numberOfThreads * sizeof(double));
			perfmon_results[i].counters = (double**) malloc(numberOfThreads * sizeof(double*));

			for (j=0;j<numberOfThreads; j++)
			{
				perfmon_results[i].time[j] = 0.0;
				perfmon_results[i].cycles[j] = 0.0;
				perfmon_results[i].instructions[j] = 0.0;
				perfmon_results[i].counters[j] = (double*) malloc(NUM_PMC * sizeof(double));
				for (k=0;k<NUM_PMC; k++)
				{
					perfmon_results[i].counters[j][k] = 0.0;
				}
			}
		}

        sl.src = src;
        sl.line = 0;
        bsplitcb (src, (char) '\n', bstrchr(src,10)+1, lineCb, &sl);

		fclose (fp);
		bdestroy (src);
	}
}

#if 0
static void
printResultTable()
{
    int thread_id;
    int i,j;
    TableContainer* table;
    bstrList* labelStrings;
    bstring label;
    int numRows = 2; /* Default Rows instruction, cycles */

    for (j=0;j<NUM_PMC;j++) 
    {
        if (threadData[0].counters[j].init == TRUE) 
        {
            numRows++;
        }
    }
    labelStrings = bstrListCreate();
    bstrListAlloc(labelStrings, perfmon_numThreads+1);
    label = bfromcstr("Event");
    labelStrings->entry[0] = bstrcpy(label);
    labelStrings->qty++;

    for (i=0; i<perfmon_numThreads;i++)
    {
        label = bformat("core %d",threadData[i].cpu_id);
        labelStrings->entry[1+i] = bstrcpy(label);
        labelStrings->qty++;
    }

    table = asciiTable_allocate(numRows, perfmon_numThreads+1,labelStrings);

    for(int regionId=0; regionId < perfmon_numRegions; regionId++)
    {
        asciiTable_setCurrentRow(table,0);
        printf ("Region  %s \n",bdata(perfmon_results[regionId].tag));
        label = bfromcstr("Instructions");
        labelStrings->entry[0] = bstrcpy(label);
        for (i=0; i<perfmon_numThreads;i++)
        {
            label = bformat("%e", perfmon_results[regionId].instructions[i]);
            labelStrings->entry[1+i] = bstrcpy(label);
        }
        asciiTable_appendRow(table,labelStrings);

        label = bfromcstr("Cycles");
        labelStrings->entry[0] = bstrcpy(label);
        for (i=0; i<perfmon_numThreads;i++)
        {
            label = bformat("%e", perfmon_results[regionId].cycles[i]);
            labelStrings->entry[1+i] = bstrcpy(label);
        }
        asciiTable_appendRow(table,labelStrings);

        for (j=0;j<NUM_PMC;j++) 
        {
            if (threadData[0].counters[j].init == TRUE) 
            {
                labelStrings->entry[0] = bstrcpy(threadData[0].counters[j].label);

                for (i=0;i<perfmon_numThreads;i++) 
                {
                    label = bformat("%e", perfmon_results[regionId].counters[i][j]);
                    labelStrings->entry[1+i] = bstrcpy(label);
                }

                asciiTable_appendRow(table,labelStrings);
            }
        }

        asciiTable_print(table);
    }
}
#endif


static void
printResultTable(PerfmonResultTable* tableData)
{
    int i,j;
    TableContainer* table;
    bstrList* labelStrings;
    bstring label;

    table = asciiTable_allocate(tableData->numRows,
            tableData->numColumns,
            tableData->header);

    for (i=0; i<tableData->numRows; i++)
    {
        labelStrings->entry[0] = bstrcpy(tableData->rows[i].label);
        for (j=0; j<(tableData->numColumns-1);j++)
        {
            label = bformat("%e", tableData->rows[i].value[j]);
            labelStrings->entry[1+j] = bstrcpy(label);
        }
        asciiTable_appendRow(table,labelStrings);
    }

    asciiTable_print(table);
//    asciiTable_free(table);
}


static PerfmonGroup
perfmon_getGroupId(bstring groupStr)
{
    PerfmonGroup group = NOGROUP;

    for (int i=0; i<numGroups; i++)
    {
        if (biseqcstr(groupStr,group_map[i].key)) 
        {
            group = group_map[i].index;
        }
    }

    return group;
}


void
setupEventSet(PerfmonEventSet* set)
{
    PerfmonCounterIndex index;
    uint32_t event;
    uint32_t umask;

    for (i=0; i<set.numberOfEvents; i++)
    {
        if (!perfmon_getIndex(set.events[i].reg, &index))
        {
            fprintf (stderr,"ERROR: Counter register %s not supported!\n",
                    set.events[i].reg->data);
            exit (EXIT_FAILURE);
        }

        if (!perfmon_getEvent(set.events[i].eventName, &event, &umask))
        {
            fprintf (stderr,"ERROR: Event %s not found for current architecture\n",event_str->data );
            exit (EXIT_FAILURE);
        }

        if (perfmon_setupCounter(event, umask, index))
        {
            printf("%s\n",bdata(set.events[i].eventName));
        }
        else
        {
            fprintf (stderr,"ERROR: Performance event %s not supported!\n",
                    bdata(set.events[i].eventName));
            exit (EXIT_FAILURE);
        }
    }
}



/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ################## */

int
perfmon_getEvent(bstring event_str, uint32_t* event, uint32_t* umask)
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

#define bstrListAdd(id,name) \
    label = bfromcstr(#name);  \
    header->entry[id] = bstrcpy(label);  \
    header->qty++;


void 
perfmon_printMarkerResults()
{
    int i;
    PerfmonResultTable tableData;
    bstrList* header;
    bstring label;
    int numRows = 2; /* Default Rows instruction, cycles */
    int numColumns = (perfmon_numThreads+1);

    readMarkerFile("/tmp/likwid_results.txt");

    for (j=0;j<NUM_PMC;j++) 
    {
        if (threadData[0].counters[j].init == TRUE) 
        {
            numRows++;
        }
    }
    header = bstrListCreate();
    bstrListAlloc(header, numColumns);
    bstrListAdd(0,Event);

    for (i=0; i<perfmon_numThreads;i++)
    {
        bstrListAdd(0,Event);
        label = bformat("core %d",threadData[i].cpu_id);
        header->entry[1+i] = bstrcpy(label);
        header->qty++;
    }

    tableData.numRows = numRows;
    tableData.numColumns = numColumns;
    tableData.header = header;
    tableData.rows = (PerfmonResult*) malloc(numRows*sizeof(PerfmonResult));

    for (i=0; i<numRows; i++)
    {
        tableData.rows[i].label = bfromcstr("Instructions");



    }


    printResultTable(&tableData);



 //   printResults(groupSet);
}


void 
perfmon_printResults()
{
#if 0
    /* Fill LikwidREsults */

    for (thread_id=0;thread_id<perfmon_numThreads;thread_id++) 
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
#endif
}


void
perfmon_setupEventSet(bstring eventString)
{
    PerfmonEventSet set;
    PerfmonGroup group = getGroupId(eventString);

    if (groupSet == NOGROUP)
    {
        /* eventString is a custom eventSet */
        bstr_to_eventset(&set, eventString);
    }
    else
    {
        /* eventString is a group */
        setupGroup(&set, group);
    }

    setupEventSet(&set);
}

void
perfmon_startAllCounters(void)
{
    int i;

    for (i=0;i<perfmon_numThreads;i++)
    {
        perfmon_startCountersThread(i);
    }

    timer_startCycles(&timeData);
}

void
perfmon_stopAllCounters(void)
{
    int i;

    timer_stopCycles(&timeData);
    for (i=0;i<perfmon_numThreads;i++)
    {
        perfmon_stopCountersThread(i);
    }

    perfmon_setCycles(timer_printCycles(&timeData));
}

void
perfmon_startCounters(int thread_id)
{
    perfmon_startCountersThread(thread_id);

}

void
perfmon_stopCounters(int thread_id)
{
    perfmon_stopCountersThread(thread_id);

}

void
perfmon_initEventset(PerfmonEventSet* set)
{
    int i;

    for (i=0;i<set->numberOfEvents; i++)
    {
        if (!perfmon_getEvent(set->events[i].eventName,
                    &set->events[i].event.event_id,
                    &set->events[i].event.umask))
        {
            printf("ERROR: Event %s not found for current architecture\n",
                    set->events[i].eventName->data);
            exit(EXIT_FAILURE);
        }
        if (!perfmon_getIndex(set->events[i].reg,
                    &set->events[i].index))
        {
            printf("ERROR: Register %s not found for current architecture\n",
                    set->events[i].reg->data);
            exit(EXIT_FAILURE);
        }

        set->events[i].results = (double*) malloc(perfmon_numThreads* sizeof(double));
        set->events[i].time = (double*) malloc(perfmon_numThreads* sizeof(double));
        set->events[i].results[0] = 0.0;
    }
}



void
perfmon_init(int numThreads_local, int threads[])
{
    int i;

    perfmon_numThreads = numThreads_local;
    threadData = (PerfmonThread*) malloc(perfmon_numThreads * sizeof(PerfmonThread));

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
                    printResults = perfmon_print_results_pm;
                    perfmon_printAvailableGroups = perfmon_printGroups_pm;
                    perfmon_startCountersThread = perfmon_startCountersThread_pm;
                    perfmon_stopCountersThread = perfmon_stopCountersThread_pm;
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
                    printResults = perfmon_printResults_core2;
                    perfmon_printAvailableGroups = perfmon_printGroups_core2;
                    perfmon_startCountersThread = perfmon_startCountersThread_core2;
                    perfmon_stopCountersThread = perfmon_stopCountersThread_core2;
                    perfmon_getIndex = perfmon_getIndex_core2;
                    perfmon_setupCounterThread = perfmon_setupCounterThread_core2;
                    perfmon_setupReport = perfmon_setupReport_core2;
                    perfmon_printReport = perfmon_printReport_core2;
                    break;

                case NEHALEM_BLOOMFIELD:

                case NEHALEM_LYNNFIELD:

                    eventHash = nehalem_arch_events;
                    num_arch_events = NUM_ARCH_EVENTS_NEHALEM;

                    initThreadArch = perfmon_init_nehalem;
                    getGroupId = perfmon_getGroupId_nehalem;
                    setupGroupThread = perfmon_setupGroupThread_nehalem;
                    printResults = perfmon_printResults_nehalem;
                    perfmon_printAvailableGroups = perfmon_printGroups_nehalem;
                    perfmon_startCountersThread = perfmon_startCountersThread_nehalem;
                    perfmon_stopCountersThread = perfmon_stopCountersThread_core2;
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
            printResults = perfmon_printResults_k8;
            perfmon_printAvailableGroups = perfmon_printGroups_k8;
            perfmon_startCountersThread = perfmon_startCountersThread_k10;
            perfmon_stopCountersThread = perfmon_stopCountersThread_k10;
            break;


        case K10_FAMILY:
            eventHash = k10_arch_events;
            num_arch_events = NUM_ARCH_EVENTS_K10;

            initThreadArch = perfmon_init_k10;
            getGroupId = perfmon_getGroupId_k10;
            setupGroupThread = perfmon_setupGroupThread_k10;
            printResults = perfmon_printResults_k10;
            perfmon_printAvailableGroups = perfmon_printGroups_k10;
            perfmon_startCountersThread = perfmon_startCountersThread_k10;
            perfmon_stopCountersThread = perfmon_stopCountersThread_k10;
            break;

        default:
            fprintf(stderr, "Unsupported Processor!\n");
            exit(EXIT_FAILURE);
            break;
    }


    for (i=0;i<perfmon_numThreads;i++) 
    {
        initThread(i,threads[i]);
    }
}
