/*
 * ===========================================================================
 *
 *       Filename:  perfCtrMain.c
 *
 *    Description:  An application to read out performance counter registers
 *                  on Intel Core, Nehalem and AMD K10 processors.
 *
 *        Version:  1.0
 *        Created:  08/13/2009
 *       Revision:  none
 *
 *         Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *        Company:  RRZE Erlangen
 *        Project:  HpcUtil
 *      Copyright:  Copyright (c) 2009, Jan Treibig
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
printf("perfCtr --  Version 0.4\n"); \
printf("\n"); \
printf("Supported Options:\n"); \
printf("-h\t Help message\n"); \
printf("-v\t verbose output\n"); \
printf("-i\t print cpu info\n"); \
printf("-m\t use markers inside code \n"); \
printf("-g\t performance group  or event tag\n"); \
printf("-a\t list available performance groups\n"); \
printf("-t\t comma separated core ids to measure\n\n")

int main (int argc, char** argv)
{ 
    int optInfo = 0;
    int optPrintGroups = 0;
    int optThreaded = 0;
    int optUncoreEvent = 0;
    int optUseMarker = 0;
    int cpu_id = 0;
    int c;
    char * cmd_str;
    char * eventString = NULL;
    PerfmonGroup group= FLOPS_DP;
    char *token, *saveptr, *str;
    char *delimiter = ",";
    int num_threads=0;
    int threads[MAX_NUM_THREADS];

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
                optThreaded = 1;
                str = optarg;
                token = str;
                while (token)
                {
                    token = strtok_r(str,delimiter,&saveptr);
                    str = NULL;
                    if (token) 
                    {
                        num_threads++;
                        threads[num_threads-1] = atoi(token);
                    }
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

    timer_init();
    cpuid_init();
	cpuFeatures_init(0);

    if (cpuFeatureFlags.speedstep)
    {
        fprintf (stderr, "Speedstep is enabled!\nThis produces inaccurate timing measurements.\n");
        fprintf (stderr, "For reliable clock measurements disable speedstep.\n");
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

