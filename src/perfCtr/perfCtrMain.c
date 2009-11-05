/*
 * ===========================================================================
 *
 *       Filename:  perfCtrMain.c
 *
 *    Description:  An application to read out performance counter registers
 *                  on Intel Core, Nehalem and AMD K10 processors.
 *
 *        Version:  0.1
 *        Created:  08/13/2009
 *
 *         Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *        Company:  RRZE Erlangen
 *        Project:  HpcUtil
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

#include <types.h>
#include <timer.h>
#include <cpuid.h>
#include <cpuFeatures.h>
#include <perfmon.h>

#define MAX_NUM_THREADS 100

#define HELP_MSG \
printf("perfCtr --  Version 0.1\n"); \
printf("\n"); \
printf("Supported Options:\n"); \
printf("-h\t Help message\n"); \
printf("-v\t verbose output\n"); \
printf("-i\t print cpu info\n"); \
printf("-m\t use markers inside code \n"); \
printf("-g\t performance group  or event tag\n"); \
printf("-a\t list available performance groups\n"); \
printf("-t\t comma separated core ids to measure\n\n")

/* the next two functions were copied and adopted from 
 * the taskset application in linux-util package */
static const char *nexttoken(const char *str,  int sep)
{
    if (str)
    {
        str = strchr(str, sep);
    }

    if (str)
    {
        str++;
    }

    return str;
}

static int cstr_to_cpuset(int* threads,  const char* str)
{
    int numThreads=0;
    const char *p, *q;
    q = str;

    while (p = q, q = nexttoken(q, ','), p) 
    {
        unsigned int rangeBegin;
        unsigned int rangeEnd;
        const char *c1, *c2;

        if (sscanf(p, "%u", &rangeBegin) < 1)
        {
            return 0;
        }

        rangeEnd = rangeBegin;

        c1 = nexttoken(p, '-');
        c2 = nexttoken(p, ',');

        if (c1 != NULL && (c2 == NULL || c1 < c2)) 
        {
            if (sscanf(c1, "%u", &rangeEnd) < 1)
            {
                return 0;
            }
            else if(rangeEnd > 1000)
            {
                return 0;
            }
        }

        if (!(rangeBegin <= rangeEnd))
        {
            return 0;
        }

        while (rangeBegin <= rangeEnd) {
            numThreads++;
            threads[numThreads-1] = rangeBegin;
            rangeBegin++;
        }
    }

    return numThreads;

}

int main (int argc, char** argv)
{ 
    int optInfo = 0;
    int optPrintGroups = 0;
    int optUncoreEvent = 0;
    int optUseMarker = 0;
    int cpu_id = 0;
    int c;
    char * cmd_str;
    char * eventString = NULL;
    PerfmonGroup group= FLOPS_DP;
    int num_threads=0;
    /* It should be checked for size to prevent buffer overflow on threads */
    int threads[MAX_NUM_THREADS];
    int i,j;

    if (argc ==  1) { HELP_MSG; }

    while ((c = getopt (argc, argv, "t:g:u:p:hmvai")) != -1)
    {
        switch (c)
        {
            case 'h':
                HELP_MSG;
                exit (EXIT_SUCCESS);    
            case 'g':
                eventString = (char*) malloc((strlen(optarg)+10)*sizeof(char));
                strcpy(eventString,optarg);
                break;
            case 't':
                num_threads = cstr_to_cpuset(threads, optarg);

                if(!num_threads)
                {
                    fprintf (stderr, "ERROR: Failed to parse cpu list.\n");
                    exit(EXIT_FAILURE);
                }

                break;
            case 'u':
#if 0
                optUncoreEvent = 1;
                customEvent = optarg;
                group = NOGROUP;
#endif
                break;
            case 'v':
                perfmon_verbose = 1;
                break;
            case 'a':
                optPrintGroups = 1;
                break;
            case 'm':
                optUseMarker = 1;
                break;
            case 'i':
                optInfo = 1;
                perfmon_verbose = 1;
                break;
            case '?':
                if (isprint (optopt))
                {
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                }
                else
                {
                    fprintf (stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
                }
                return EXIT_FAILURE;
            default:
                HELP_MSG;
        }
    }

    for (i = 0; i< num_threads;i++)
    {
        for (j = 0; j< num_threads;j++)
        {
            if(i != j && threads[i] == threads[j])
            {
                fprintf (stderr, "ERROR: Cpu list is not unique.\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    timer_init();
    cpuid_init();


    if( cpuid_info.family == P6_FAMILY ) 
        cpuFeatures_init(0);
    {

        if (cpuFeatureFlags.speedstep)
        {
            fprintf (stderr, "Speedstep is enabled!\nThis produces inaccurate timing measurements.\n");
            fprintf (stderr, "For reliable clock measurements disable speedstep.\n");
        }
    }

    printf(HLINE);
    printf("CPU name:\t%s \n",cpuid_info.name);
    printf("CPU clock:\t%llu Hz \n", cpuid_info.clock);

    if (perfmon_verbose) 
    {
        printf("CPU family:\t%u \n",cpuid_info.family);
        printf("CPU model:\t%u \n", cpuid_info.model);
        printf("CPU stepping:\t%u \n", cpuid_info.stepping);
        printf("CPU features:\t%s \n", cpuid_info.features);

        if( cpuid_info.family == P6_FAMILY && cpuid_info.perf_version) 
        {
            printf(HLINE);
            printf("PERFMON version:\t%u \n",cpuid_info.perf_version);
            printf("PERFMON number of counters:\t%u \n",cpuid_info.perf_num_ctr);
            printf("PERFMON width of counters:\t%u \n",cpuid_info.perf_width_ctr);
            printf("PERFMON number of fixed counters:\t%u \n",cpuid_info.perf_num_fixed_ctr);
        }
    }
    printf(HLINE);

    if (optInfo)
    {
        exit (EXIT_SUCCESS);
    }

    perfmon_init(num_threads, threads);

    if (optPrintGroups)
    {
        perfmon_printAvailableGroups();
        exit (EXIT_SUCCESS);    
    }

    if (optind == argc) 
    {
        printf("NOTICE: You have to specify a program to measure as argument!\n");
        exit (EXIT_SUCCESS);
    }

    if (eventString == NULL)
    {
        eventString = "FLOPS_DP";
    }

    if (perfmon_setupGroup(eventString)) 
    {
        printf("Measuring Performance group: %s\n", eventString);
    }
    else
    {
        group = NOGROUP;
    }


    cmd_str = (char*) malloc((strlen(argv[optind])+200)*sizeof(char));
    sprintf(cmd_str,"%s",argv[optind]);

    if (perfmon_verbose) printf("Executing: %s \n",cmd_str);


    if (group == NOGROUP) 
    {
        if (optUncoreEvent) 
        {
            perfmon_setupCounter(PMCU0,eventString);
        }
        else 
        {
            if (perfmon_setupCounter(PMC0,eventString))
            {
                printf("Measuring Performance event: %s\n", eventString);
            }
            else
            {
                printf("ERROR: Performance event %s not supported!\n", eventString);
                exit (EXIT_FAILURE);
            }
        }
    }
    printf(HLINE);

    if (!optUseMarker)
    {
        perfmon_startAllCounters();
    }
    if (system(cmd_str) == EOF)
    {
        fprintf(stderr, "Failed to execute %s!\n", cmd_str);
        exit(EXIT_FAILURE);
    }

    perfmon_stopAllCounters();
    if (optUseMarker)
    {
        perfmon_getCycles();
    }
    perfmon_printResults();

    return EXIT_SUCCESS;
}

