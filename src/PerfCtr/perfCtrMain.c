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
printf("-g\t performance group [STD FLOPS_SP L1 L2 L3 MEM DATA BRANCH TLB CPI] or event tag\n"); \
printf("-t\t comma separated core ids to measure\n\n"); \
exit(0);

int main (int argc, char** argv)
{ 
    int optInfo = 0;
    int optThreaded = 0;
    int optUncoreEvent = 0;
    int optUseMarker = 0;
    int cpu_id = 0;
    int c;
    char * cmd_str;
    char * customEvent;
    PerfmonGroup group= STD;
    char *token, *saveptr, *str;
    char *delimiter = ",";
    int num_threads=0;
    int threads[MAX_NUM_THREADS];

    if (argc ==  1) { HELP_MSG }

    while ((c = getopt (argc, argv, "t:g:u:p:hmvi")) != -1)
    {
        switch (c)
        {
            case 'h':
                HELP_MSG
                    exit (EXIT_SUCCESS);    
            case 'g':
                if (!strcmp("STD",optarg)) 
                {
                    group = STD;
                }
                else if (!strcmp("FLOPS_SP",optarg)) 
                {
                    group = FLOPS_SP;
                }
                else if (!strcmp("L1",optarg)) 
                {
                    group = L1;
                }
                else if (!strcmp("L2",optarg)) 
                {
                    group = L2;
                }
                else if (!strcmp("L3",optarg)) 
                {
                    group = L3;
                }
                else if (!strcmp("MEM",optarg)) 
                {
                    group = MEM;
                }
                else if (!strcmp("DATA",optarg)) 
                {
                    group = DATA;
                }
                else if (!strcmp("BRANCH",optarg)) 
                {
                    group = BRANCH;
                }
                else if (!strcmp("TLB",optarg)) 
                {
                    group = TLB;
                }
                else if (!strcmp("CPI",optarg)) 
                {
                    group = CPI;
                }
                else
                {
                    customEvent = (char*) malloc((strlen(optarg)+10)*sizeof(char));
                    strcpy(customEvent,optarg);
                    group = NOGROUP;
                }
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
                optUncoreEvent = 1;
                customEvent = optarg;
                group = NOGROUP;
                break;
            case 'v':
                perfmon_verbose = 1;
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
                HELP_MSG
        }
    }

    timer_init();
    cpuid_init();
    printf(HLINE);
    printf("CPU name:\t%s \n",cpuid_info.name);
    printf("CPU clock:\t%llu Hz \n", cpuid_info.clock);
    if (perfmon_verbose) 
    {
        printf("CPU family:\t%u \n",cpuid_info.family);
        printf("CPU model:\t%u \n", cpuid_info.model);
        printf("CPU stepping:\t%u \n", cpuid_info.stepping);
        printf("CPU features:\t%s \n", cpuid_info.features);

        if( cpuid_info.family == P6_FAMILY) 
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
        return EXIT_SUCCESS;
    }

    if (optind == argc) 
    {
        printf("WARNING: You have to specify a program to measure as argument!\n");
        return EXIT_SUCCESS;
    }

    cmd_str = (char*) malloc((strlen(argv[optind])+200)*sizeof(char));
    sprintf(cmd_str,"%s",argv[optind]);

    if (perfmon_verbose) printf("Executing: %s \n",cmd_str);

    perfmon_init(num_threads, threads);

    if (group != NOGROUP) 
    {
        perfmon_setupGroup(group);
    }
    else 
    {
        if (optUncoreEvent) 
        {
            perfmon_setupCounter(PMCU0,customEvent);
        }
        else 
        {
            perfmon_setupCounter(PMC0,customEvent);
        }
    }

    if (!optUseMarker)
    {
        perfmon_startAllCounters();
    }
    system(cmd_str);
    perfmon_stopAllCounters();
    if (optUseMarker)
    {
        perfmon_getCycles();
    }
    perfmon_printResults();

    return EXIT_SUCCESS;
}

