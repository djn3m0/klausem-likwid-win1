/*
 * ===========================================================================
 *
 *       Filename:  cpuFeaturesMain.c
 *
 *    Description:  An application to read out and set the feature flag
 *                  register on Intel Core 2 processors.
 *
 *        Version:  <VERSION>
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
#include <msr.h>
#include <cpuid.h>
#include <cpuFeatures.h>


#define HELP_MSG \
printf("\nlikwid-features --  Version  %d.%d \n\n",VERSION,RELEASE); \
printf("A tool to print and toggle the feature flag msr on Intel CPUS.\n"); \
printf("Supported Features: HW_PREFETCHER, CL_PREFETCHER, DCU_PREFETCHER, IP_PREFETCHER.\n\n"); \
printf("Options:\n"); \
printf("-h\t Help message\n"); \
printf("-v\t Version information\n"); \
printf("-i\t print cpu features\n"); \
printf("-s <FEATURE>\t set cpu feature \n"); \
printf("-u <FEATURE>\t unset cpu feature \n"); \
printf("-t <ID>\t core id\n\n")

#define VERSION_MSG \
printf("likwid-features  %d.%d \n\n",VERSION,RELEASE)

int main (int argc, char** argv)
{ 
    int optSetFeature = 0;
    int cpuId = 0;
    int c;
    CpuFeature feature = HW_PREFETCHER ;

    while ((c = getopt (argc, argv, "t:s:u:hv")) != -1)
    {
        switch (c)
        {
            case 'h':
                HELP_MSG;
                exit (EXIT_SUCCESS);    
            case 'v':
                VERSION_MSG;
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

    cpuid_init();

    printf(HLINE);
    printf("CPU name:\t%s \n",cpuid_info.name);
//    printf("CPU clock:\t%llu Hz \n", LLU_CAST cpuid_info.clock);
    printf("CPU core id:\t%d \n", cpuId);

    if (cpuid_info.family != P6_FAMILY)
    {
        fprintf (stderr, "likwid-features only supports Intel P6 based processors!\n");
        exit(EXIT_FAILURE);
    }

    msr_check();
	cpuFeatures_init(cpuId);
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

