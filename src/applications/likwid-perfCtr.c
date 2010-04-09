/*
 * ===========================================================================
 *
 *       Filename:  perfCtrMain.c
 *
 *    Description:  An application to read out performance counter registers
 *                  on x86 processors
 *
 *        Version:  <VERSION>
 *        Created:  08/13/2009
 *
 *         Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *        Company:  RRZE Erlangen
 *        Project:  LIKWID
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
#include <ctype.h>

#include <types.h>
#include <msr.h>
#include <timer.h>
#include <cpuid.h>
#include <cpuFeatures.h>
#include <perfmon.h>
#include <bstrlib.h>
#include <strUtil.h>
#include <multiplex.h>


#define HELP_MSG \
printf("likwid-perfCtr --  Version  %d.%d \n\n",VERSION,RELEASE); \
printf("\n"); \
printf("Supported Options:\n"); \
printf("-h\t Help message\n"); \
printf("-v\t Version information\n"); \
printf("-V\t verbose output\n"); \
printf("-i\t print cpu info\n"); \
printf("-r\t Generate performance report\n"); \
printf("-m\t use markers inside code \n"); \
printf("-g\t performance group  or event tag\n"); \
printf("-a\t list available performance groups\n"); \
printf("-c\t comma separated core ids to measure\n\n")


#define VERSION_MSG \
printf("likwid-perfCtr  %d.%d \n\n",VERSION,RELEASE);



int main (int argc, char** argv)
{ 
    int optInfo = 0;
    int optPrintGroups = 0;
    int optUseMarker = 0;
    int optReport = 0;
    int c;
    bstring eventString = bformat("FLOPS_DP");
    bstring  argString;
    int numThreads=0;
    int threads[MAX_NUM_THREADS];
    MultiplexCollections set;
    int i,j;

    if (argc ==  1) 
    {
        HELP_MSG;
        exit (EXIT_SUCCESS);    
    }

    while ((c = getopt (argc, argv, "+c:g:p:hvmVair")) != -1)
    {
        switch (c)
        {
            case 'h':
                HELP_MSG;
                exit (EXIT_SUCCESS);    
            case 'v':
                VERSION_MSG;
                exit (EXIT_SUCCESS);    
            case 'g':
                if (! (argString = bSecureInput(200,optarg)))
                {
                    fprintf(stderr,"Failed to read argument string!\n");
                    exit(EXIT_FAILURE);
                }

                eventString = bstrcpy(argString);
                bdestroy(argString);
                break;
            case 'c':
                if (! (argString = bSecureInput(200,optarg)))
                {
                    fprintf(stderr,"Failed to read argument string!\n");
                    exit(EXIT_FAILURE);
                }

                numThreads = bstr_to_cpuset(threads, argString);

                if(!numThreads)
                {
                    fprintf (stderr, "ERROR: Failed to parse cpu list.\n");
                    exit(EXIT_FAILURE);
                }
                bdestroy(argString);
                break;
            case 'V':
                perfmon_verbose = 1;
                break;
            case 'a':
                optPrintGroups = 1;
                break;
            case 'm':
                optUseMarker = 1;
                break;
            case 'r':
                optReport = 1;
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
                exit (EXIT_SUCCESS);    
        }
    }

    for (i = 0; i< numThreads;i++)
    {
        for (j = 0; j< numThreads;j++)
        {
            if(i != j && threads[i] == threads[j])
            {
                fprintf (stderr, "ERROR: Cpu list is not unique.\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    msr_check();
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
    printf("CPU type:\t%s \n",cpuid_info.name);
    printf("CPU clock:\t%3.2f GHz \n",  (float) cpuid_info.clock * 1.E-09);

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
    perfmon_init(numThreads, threads);

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

    if(optReport)
    {
        perfmon_setupReport(&set);
        multiplex_init(&set);
    }
    else
    {
        perfmon_setupEventSet(eventString);
    }

    printf(HLINE);

	argv +=  optind;
    if (perfmon_verbose) printf("Executing: %s \n",argv[0]);

    if (optReport)
    {
        multiplex_start(optUseMarker);
    }
    else if (!optUseMarker)
    {
        perfmon_startAllCounters();
    }

    if (system(argv[0]) == EOF)
    {
        fprintf(stderr, "Failed to execute %s!\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (optReport)
    {
        multiplex_stop();
        perfmon_printReport(&set);
    }
    else
	{
		if (!optUseMarker)
		{
			perfmon_stopAllCounters();
			perfmon_printCounterResults();
		}
		else
		{
			perfmon_printMarkerResults();
		}
	}

    return EXIT_SUCCESS;
}

