/*
 * ===========================================================================
 *
 *       Filename:  cpuFeaturesMain.c
 *
 *    Description:  An application to read out and set the feature flag
 *                  register on Intel Core 2 processors.
 *
 *        Version:  1.0
 *        Created:  08/13/2009
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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#include <types.h>
#include <timer.h>
#include <cpuid.h>
#include <cpuFeatures.h>


#define HELP_MSG \
printf("\ncpuFeatures --  Version 0.2\n\n"); \
printf("A tool to print and toggle the feature flag msr on Intel CPUS.\n"); \
printf("Supported Features: HW_PREFETCHER, CL_PREFETCHER, DCU_PREFETCHER, IP_PREFETCHER.\n\n"); \
printf("Options:\n"); \
printf("-h\t Help message\n"); \
printf("-i\t print cpu features\n"); \
printf("-s <FEATURE>\t set cpu feature \n"); \
printf("-u <FEATURE>\t unset cpu feature \n"); \
printf("-t <ID>\t core id\n\n"); \
exit(0);

int main (int argc, char** argv)
{ 
    int optInfo = 0;
    int optSetFeature = 0;
    int cpuId = 0;
    int c;
    CpuFeature feature = HW_PREFETCHER ;

    if (argc ==  1) { HELP_MSG }

    while ((c = getopt (argc, argv, "t:s:u:hi")) != -1)
    {
        switch (c)
        {
            case 'h':
                HELP_MSG
                    exit (EXIT_SUCCESS);    
            case 'u':
                optSetFeature = 2;
            case 's':
                if (!strcmp("HW_PREFETCHER",optarg)) 
                {
                    feature = HW_PREFETCHER;
                }
                else if (!strcmp("CL_PREFETCHER",optarg)) 
                {
                    feature = CL_PREFETCHER;
                }
                else if (!strcmp("DCU_PREFETCHER",optarg)) 
                {
                    feature = DCU_PREFETCHER;
                }
                else if (!strcmp("IP_PREFETCHER",optarg)) 
                {
                    feature = IP_PREFETCHER;
                }
                else
                {
                    fprintf(stderr,"Feature not supported!\n");
                    exit(EXIT_FAILURE);
                }

                if (!optSetFeature)
                {
                    optSetFeature = 1;
                }
                break;
            case 't':
                cpuId = atoi(optarg);

                break;
            case 'i':
                optInfo = 1;
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

	cpuFeatures_init(cpuId);

    if (cpuFeatureFlags.speedstep)
    {
        fprintf (stderr, "Speedstep is enabled!\nThis produces inaccurate timing measurements.\n");
        fprintf (stderr, "For reliable clock measurements disable speedstep.\n");
    }

    timer_init();
    cpuid_init();

    printf(HLINE);
    printf("CPU name:\t%s \n",cpuid_info.name);
    printf("CPU clock:\t%llu Hz \n", LLU_CAST cpuid_info.clock);
    printf("CPU core id:\t%d \n", cpuId);

    if (cpuid_info.family != P6_FAMILY)
    {
        fprintf (stderr, "cpuFeature only supports Intel P6 based processors\n");
        exit(EXIT_FAILURE);
    }

    cpuFeatures_print(cpuId);

    if (optSetFeature == 1)
    {
        printf(SLINE);
        cpuFeatures_enable(cpuId, feature);
        printf(SLINE);
    }
    else if (optSetFeature == 2)
    {
        printf(SLINE);
        cpuFeatures_disable(cpuId, feature);
        printf(SLINE);
    }

    return EXIT_SUCCESS;
}

