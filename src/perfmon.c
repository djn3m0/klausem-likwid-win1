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
 *      Copyright:  Copyright (c) 2010, Jan Treibig
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
#include <strUtil.h>
#include <types.h>
#include <timer.h>
#include <msr.h>
#include <cpuid.h>
#include <perfmon.h>
#include <asciiTable.h>
#include <registers.h>
//#include <perfmon_pm_events.h>
//#include <perfmon_k8_events.h>
//#include <perfmon_k10_events.h>


/* #####   EXPORTED VARIABLES   ########################################### */

int perfmon_numArchEvents;
int perfmon_numThreads;
int perfmon_numRegions;

int perfmon_verbose=0;
PerfmonThread* threadData;
LikwidResults* perfmon_results;

/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ###################### */

static PerfmonGroup groupSet = NOGROUP;
static PerfmonEvent* eventHash;
static PerfmonCounterMap* counter_map;
static PerfmonGroupMap* group_map;
static char** group_config;

static PerfmonThread summary;
static CyclesData timeData;
static PerfmonEventSet perfmon_set;
static int perfmon_numGroups;
static int perfmon_numCounters;

/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ##################### */

static void initResultTable(PerfmonResultTable* tableData,
        bstrList* firstColumn,
        int numRows,
        int numColumns);

static void printResultTable(PerfmonResultTable* tableData);
static void initThread(int , int );

/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ######################### */

#define CHECKERROR \
        if (ret == EOF) \
        { \
            fprintf (stderr, "sscanf: Failed to read marker file!\n" ); \
            exit (EXIT_FAILURE);}

#define bstrListAdd(id,name) \
    label = bfromcstr(#name);  \
    fc->entry[id] = bstrcpy(label);  \
    fc->qty++;

#define INIT_EVENTS   \
    bstrList* fc; \
    bstring label; \
    fc = bstrListCreate(); \
    bstrListAlloc(fc, numRows+1); \
    bstrListAdd(0,Event); \
    for (i=0; i<numRows; i++) \
    { \
        fc->entry[1+i] = \
           bfromcstr(perfmon_set.events[i].event.name); } 

#define INIT_BASIC  \
    fc = bstrListCreate(); \
    bstrListAlloc(fc, numRows+1); \
    bstrListAdd(0,Metric);

//#include <perfmon_pm.h>
#include <perfmon_core2.h>
#include <perfmon_nehalem.h>
//#include <perfmon_k8.h>
//#include <perfmon_k10.h>

/* #####  EXPORTED  FUNCTION POINTERS   ################################### */
void (*perfmon_startCountersThread) (int thread_id);
void (*perfmon_stopCountersThread) (int thread_id);
void (*perfmon_setupCounterThread) (int thread_id,
        uint32_t umask, uint32_t event, PerfmonCounterIndex index);
void (*perfmon_setupReport) (MultiplexCollections* collections);
void (*perfmon_printReport) (MultiplexCollections* collections);

/* #####   FUNCTION POINTERS  -  LOCAL TO THIS SOURCE FILE ################ */

static void (*initThreadArch) (PerfmonThread *thread);
void (*printDerivedMetrics) (PerfmonGroup group);

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ########### */

static int  getIndex (bstring reg, PerfmonCounterIndex* index)
{
    int i;

    for (i=0; i< perfmon_numCounters; i++)
    {
        if (biseqcstr(reg, counter_map[i].key))
        {
            *index = counter_map[i].index;
            return TRUE;
        }
    }

    return FALSE;
}

static int
getEvent(bstring event_str, PerfmonEvent* event)
{
    int i;

    for (i=0; i< perfmon_numArchEvents; i++)
    {
        if (biseqcstr(event_str, eventHash[i].name))
        {
            *event = eventHash[i];

            if (perfmon_verbose)
            {
                printf ("Found event %s : Event_id 0x%02X Umask 0x%02X \n",
                        bdata(event_str), event->eventId, event->umask);
            }
            return TRUE;
        }
    }

    return FALSE;
}

static void
initThread(int thread_id, int cpu_id)
{
    int i;

    for (i=0; i<NUM_PMC; i++)
    {
        threadData[thread_id].counters[i].init = FALSE;
    }

    threadData[thread_id].processorId = cpu_id;
    initThreadArch(&threadData[thread_id]);

}

struct cbsScan{
	/* Parse state */
	bstring src;
	int line;
};

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

 /* :TODO:04/09/10 09:15:38:jt: 
 *  the current provisorical solution is to read the file in as LikwidResults
 * and then fill the results into a PerfmonEventSet. Later this should be directly 
 * put into a perfmonEventSet from the beginning */
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


static void
printResultTable(PerfmonResultTable* tableData)
{
    int i,j;
    TableContainer* table;
    bstrList* labelStrings;
    bstring label;

    table = asciiTable_allocate(tableData->numRows,
            tableData->numColumns+1,
            tableData->header);

    bstrListAlloc(labelStrings, tableData->numColumns+1);

    for (i=0; i<tableData->numRows; i++)
    {
        labelStrings->qty = 0;
        labelStrings->entry[0] = bstrcpy(tableData->rows[i].label);
        labelStrings->qty++;

        for (j=0; j<(tableData->numColumns);j++)
        {
            label = bformat("%e", tableData->rows[i].value[j]);
            labelStrings->entry[1+j] = bstrcpy(label);
            labelStrings->qty++;
        }
        asciiTable_appendRow(table,labelStrings);
    }

    asciiTable_print(table);
//    asciiTable_free(table);
}


static int
getGroupId(bstring groupStr,PerfmonGroup* group)
{
    *group = NOGROUP;

    for (int i=0; i<perfmon_numGroups; i++)
    {
        if (biseqcstr(groupStr,group_map[i].key)) 
        {
            *group = group_map[i].index;
            return i;
        }
    }

    return -1;
}

static int
checkCounter(bstring counterName, char* limit)
{


    return TRUE;
}

static void 
initResultTable(PerfmonResultTable* tableData,
        bstrList* firstColumn,
        int numRows,
        int numColumns)
{
    int i;
    bstrList* header;
    bstring label;

    header = bstrListCreate();
    bstrListAlloc(header, numColumns+1);
    header->entry[0] = bstrcpy(firstColumn->entry[0]); header->qty++;

    for (i=0; i<perfmon_numThreads;i++)
    {
        label = bformat("core %d",threadData[i].processorId);
        header->entry[1+i] = bstrcpy(label); header->qty++;
    }

    tableData->numRows = numRows;
    tableData->numColumns = numColumns;
    tableData->header = header;
    tableData->rows = (PerfmonResult*) malloc(numRows*sizeof(PerfmonResult));

    for (i=0; i<numRows; i++)
    {
//        tableData->rows[i].label =
//           bfromcstr(perfmon_set.events[i].event.name);

        tableData->rows[i].label = firstColumn->entry[1+i];

        tableData->rows[i].value =
            (double*) malloc((numColumns)*sizeof(double));
    }
}

/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ################## */

double
perfmon_getResult(int threadId, char* counterString)
{
    bstring counter = bfromcstr(counterString);
    PerfmonCounterIndex  index;

   if (getIndex(counter,&index))
   {
       return threadData[threadId].counters[index].counterData;
   }

   fprintf (stderr, "perfmon_getResult: Failed to get counter Index!\n" );
   return 0.0;
}


void
perfmon_initEventSet(StrUtilEventSet* eventSetConfig, PerfmonEventSet* set)
{
    int i;

    set->numberOfEvents = eventSetConfig->numberOfEvents;
    set->events = (PerfmonEventSetEntry*)
        malloc(set->numberOfEvents * sizeof(PerfmonEventSetEntry));

    for (i=0; i<set->numberOfEvents; i++)
    {
        /* get register index */
        if (!getIndex(eventSetConfig->events[i].counterName,
                    &set->events[i].index))
        {
            fprintf (stderr,"ERROR: Counter register %s not supported!\n",
                    bdata(eventSetConfig->events[i].counterName));
            exit (EXIT_FAILURE);
        }

        /* setup event */
        if (!getEvent(eventSetConfig->events[i].eventName,
                    &set->events[i].event))
        {
            fprintf (stderr,"ERROR: Event %s not found for current architecture\n",
                    bdata(eventSetConfig->events[i].eventName));
            exit (EXIT_FAILURE);
        }
        
        /* is counter allowed for event */
        if (!checkCounter(eventSetConfig->events[i].counterName,
                    set->events[i].event.limit))
        {
            fprintf (stderr,"ERROR: Register %s not allowed  for event %s\n",
                    bdata(eventSetConfig->events[i].counterName),
                    bdata(eventSetConfig->events[i].eventName));
            exit (EXIT_FAILURE);
        }
    }
}

void 
perfmon_printMarkerResults()
{
    int i;
    int j;
    int region;
    PerfmonResultTable tableData;
    int numRows = perfmon_set.numberOfEvents;
    int numColumns = perfmon_numThreads;
    INIT_EVENTS;

    readMarkerFile("/tmp/likwid_results.txt");

    initResultTable(&tableData, fc, numRows, numColumns);

    for (region=0; region<perfmon_numRegions; region++)
    {
        for (i=0; i<numRows; i++)
        {
            for (j=0; j<numColumns; j++)
            {
                tableData.rows[i].value[j] =
                    perfmon_results[region].counters[j][perfmon_set.events[i].index];
            }
        }
        printResultTable(&tableData);
    }
}

void 
perfmon_printCounterResults()
{
    int i;
    int j;
    PerfmonResultTable tableData;
    int numRows = perfmon_set.numberOfEvents;
    int numColumns = perfmon_numThreads;
    INIT_EVENTS;

    initResultTable(&tableData, fc, numRows, numColumns);

    for (i=0; i<numRows; i++)
    {
        for (j=0; j<numColumns; j++)
        {
            tableData.rows[i].value[j] =
                (double) threadData[j].counters[perfmon_set.events[i].index].counterData;
        }
    }
    printResultTable(&tableData);
    printDerivedMetrics(groupSet);
}


void
perfmon_setupEventSetC(char* eventCString)
{
    bstring eventString = bfromcstralloc(400,eventCString);
    StrUtilEventSet eventSetConfig;

    groupSet = NOGROUP;
  //  bassigncstr(eventString, eventCString);
    bstr_to_eventset(&eventSetConfig, eventString);
    perfmon_initEventSet(&eventSetConfig, &perfmon_set);
    perfmon_setupCounters();
}

void
perfmon_setupEventSet(bstring eventString)
{
    int groupId;
    StrUtilEventSet eventSetConfig;

    groupId = getGroupId(eventString, &groupSet);

    if (groupSet == NOGROUP)
    {
        /* eventString is a custom eventSet */
        bstr_to_eventset(&eventSetConfig, eventString);
    }
    else
    {
        /* eventString is a group */
        bassigncstr(eventString, group_config[groupId]);
        bstr_to_eventset(&eventSetConfig, eventString);
    }

    perfmon_initEventSet(&eventSetConfig, &perfmon_set);
    perfmon_setupCounters();
}


void
perfmon_setupCounters()
{
  int i;
  int j;
  uint32_t eventId;
  uint32_t umask;
  PerfmonCounterIndex  index;

    for (j=0; j<perfmon_set.numberOfEvents; j++)
    {
        eventId = perfmon_set.events[j].event.eventId;
        umask   = perfmon_set.events[j].event.umask;
        index   = perfmon_set.events[j].index;
 //       printf("%s\n",perfmon_set.events[i].name);

        for (i=0; i<perfmon_numThreads; i++)
        {
            perfmon_setupCounterThread(i,
                    eventId,
                    umask,
                    index);
        }
    }
}

void
perfmon_startCounters(void)
{
    int i;

    for (i=0;i<perfmon_numThreads;i++)
    {
        perfmon_startCountersThread(i);
    }

    timer_startCycles(&timeData);
}

void
perfmon_stopCounters(void)
{
    int i;

    timer_stopCycles(&timeData);

    for (i=0;i<perfmon_numThreads;i++)
    {
        perfmon_stopCountersThread(i);
    }
}

void
perfmon_printAvailableGroups()
{
    int i;

    printf("Available groups on %s:\n",cpuid_info.name);

    for(i=0; i<perfmon_numGroups; i++)
    {
        printf("%s: %s\n",group_map[i].key,
                group_map[i].info);
    }
}


void
perfmon_init(int numThreads_local, int threads[])
{
    int i;

    perfmon_numThreads = numThreads_local;
    threadData = (PerfmonThread*) malloc(perfmon_numThreads * sizeof(PerfmonThread));
    cpuid_init();

    switch ( cpuid_info.family ) 
    {
        case P6_FAMILY:

            switch ( cpuid_info.model ) 
            {
#if 0
                case PENTIUM_M_BANIAS:

                case PENTIUM_M_DOTHAN:

                    eventHash = pm_arch_events;
                    perfmon_numArchEvents = perfmon_numArchEvents_PM;

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
#endif
                    break;

                case XEON_MP:

                case CORE2_65:

                case CORE2_45:

                    eventHash = core2_arch_events;
                    perfmon_numArchEvents = perfmon_numArchEventsCore2;

                    group_map = core2_group_map;
                    group_config = core2_group_config;
                    perfmon_numGroups = perfmon_numGroupsCore2;

                    counter_map = core2_counter_map;
                    perfmon_numCounters = perfmon_numCountersCore2;

                    initThreadArch = perfmon_init_core2;
                    printDerivedMetrics = perfmon_printDerivedMetricsCore2;
                    perfmon_startCountersThread = perfmon_startCountersThread_core2;
                    perfmon_stopCountersThread = perfmon_stopCountersThread_core2;
                    perfmon_setupCounterThread = perfmon_setupCounterThread_core2;
                    break;

                case NEHALEM_BLOOMFIELD:

                case NEHALEM_LYNNFIELD:

                    eventHash = nehalem_arch_events;
                    perfmon_numArchEvents = perfmon_numArchEventsNehalem;

                    group_map = nehalem_group_map;
                    group_config = nehalem_group_config;
                    perfmon_numGroups = perfmon_numGroupsNehalem;

                    counter_map = nehalem_counter_map;
                    perfmon_numCounters = perfmon_numCountersNehalem;

                    initThreadArch = perfmon_init_nehalem;
                    printDerivedMetrics = perfmon_printDerivedMetricsNehalem;
                    perfmon_startCountersThread = perfmon_startCountersThread_nehalem;
                    perfmon_stopCountersThread = perfmon_stopCountersThread_nehalem;
                    perfmon_setupCounterThread = perfmon_setupCounterThread_nehalem;
                    break;

                default:
                    fprintf(stderr, "Unsupported Processor!\n");
                    exit(EXIT_FAILURE);
                    break;
            }
            break;

#if 0
        case K8_FAMILY:
            eventHash = k8_arch_events;
            perfmon_numArchEvents = perfmon_numArchEvents_K8;

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
            perfmon_numArchEvents = perfmon_numArchEvents_K10;

            initThreadArch = perfmon_init_k10;
            getGroupId = perfmon_getGroupId_k10;
            setupGroupThread = perfmon_setupGroupThread_k10;
            printResults = perfmon_printResults_k10;
            perfmon_printAvailableGroups = perfmon_printGroups_k10;
            perfmon_startCountersThread = perfmon_startCountersThread_k10;
            perfmon_stopCountersThread = perfmon_stopCountersThread_k10;
            break;
#endif

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

