/*
 * ===========================================================================
 *
 *       Filename:  libperfctr.c
 *
 *    Description:  Marker API interface of module perfmon
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

#include <types.h>
#include <bstrlib.h>
#include <cpuid.h>
#include <msr.h>
#include <timer.h>
#include <registers.h>
#include <likwid.h>

static LikwidResults*  results;
static CyclesData*  time;
static int likwid_numberOfRegions = 0;
static int likwid_numberOfThreads = 0;

/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ################## */
void
likwid_markerInit(int numberOfThreads, int numberOfRegions)
{
    int i;
    int j;
    int k;

    cpuid_init();
    timer_init();

	likwid_numberOfThreads = numberOfThreads;
	likwid_numberOfRegions = numberOfRegions;

    time = (CyclesData*) malloc(numberOfThreads * sizeof(CyclesData));
    results = (LikwidResults*) malloc(numberOfRegions * sizeof(LikwidResults));

	// TODO: Pad data structures to prevent false shared cache lines
    for (i=0;i<numberOfRegions; i++)
    {
        results[i].cycles = (double*) malloc(numberOfThreads * sizeof(double));
        results[i].instructions = (double*) malloc(numberOfThreads * sizeof(double));
        results[i].time = (double*) malloc(numberOfThreads * sizeof(double));
        results[i].counters = (double**) malloc(numberOfThreads * sizeof(double*));

        for (j=0;j<numberOfThreads; j++)
        {
            results[i].time[j] = 0.0;
            results[i].cycles[j] = 0.0;
            results[i].instructions[j] = 0.0;
            results[i].counters[j] = (double*) malloc(NUM_PMC * sizeof(double));
            for (k=0;k<NUM_PMC; k++)
            {
                results[i].counters[j][k] = 0.0;
            }
        }
    }
}

void likwid_markerClose()
{
	int i,j,k;
    FILE *file;

    file = fopen("/tmp/likwid_results.txt","w");

	fprintf(file,"NUMTHREADS %d\n",likwid_numberOfThreads);
	fprintf(file,"NUMREGIONS %d\n",likwid_numberOfRegions);

    for (i=0;i<likwid_numberOfRegions; i++)
    {
		fprintf(file,"** BEGIN TAG %s **\n",results[i].tag->data);
        for (j=0;j<likwid_numberOfThreads; j++)
        {
            fprintf(file,"** BEGIN THREAD %d **\n",j);
            fprintf(file,"TIME %e\n",results[i].time[j]);
            fprintf(file,"CYCLES %e\n",results[i].cycles[j]);
            fprintf(file,"INSTR %e\n",results[i].instructions[j]);

            for (k=0;k<NUM_PMC; k++)
            {
                fprintf(file,"PMC %d: %e\n",k,results[i].counters[j][k]);
            }
            fprintf(file,"** END THREAD %d **\n",j);
        }
		fprintf(file,"** END TAG %s **\n",results[i].tag->data);
    }
    fclose(file);
}

void
likwid_markerStartRegion(int cpu_id)
{
    uint64_t flags = 0x0ULL;

    switch ( cpuid_info.family ) 
    {
        case P6_FAMILY:

            msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);
            msr_write(cpu_id, MSR_PERF_FIXED_CTR0, 0x0ULL);
            msr_write(cpu_id, MSR_PERF_FIXED_CTR1, 0x0ULL);

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

                    msr_write(cpu_id, MSR_PMC0 , 0x0ULL);
                    msr_write(cpu_id, MSR_PMC1 , 0x0ULL);
                    msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x300000003ULL);
                    msr_write(cpu_id, MSR_PERF_GLOBAL_OVF_CTRL, 0x300000003ULL);
                    break;

                case NEHALEM_BLOOMFIELD:

                case NEHALEM_LYNNFIELD:

                    msr_write(cpu_id, MSR_PMC0 , 0x0ULL);
                    msr_write(cpu_id, MSR_PMC1 , 0x0ULL);
                    msr_write(cpu_id, MSR_PMC2 , 0x0ULL);
                    msr_write(cpu_id, MSR_PMC3 , 0x0ULL);
                    msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x30000000FULL);
                    msr_write(cpu_id, MSR_PERF_GLOBAL_OVF_CTRL, 0x30000000FULL);
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

                    msr_write(cpu_id, MSR_AMD_PMC0 , 0x0ULL);
                    msr_write(cpu_id, MSR_AMD_PMC1 , 0x0ULL);
                    msr_write(cpu_id, MSR_AMD_PMC2 , 0x0ULL);
                    msr_write(cpu_id, MSR_AMD_PMC3 , 0x0ULL);

                    flags = msr_read(cpu_id, MSR_AMD_PERFEVTSEL0);
                    flags |= (1<<22);  /* enable flag */
                    msr_write(cpu_id, MSR_AMD_PERFEVTSEL0, flags);
                    flags = msr_read(cpu_id, MSR_AMD_PERFEVTSEL1);
                    flags |= (1<<22);  /* enable flag */
                    msr_write(cpu_id, MSR_AMD_PERFEVTSEL1, flags);
                    flags = msr_read(cpu_id, MSR_AMD_PERFEVTSEL2);
                    flags |= (1<<22);  /* enable flag */
                    msr_write(cpu_id, MSR_AMD_PERFEVTSEL2, flags);
                    flags = msr_read(cpu_id, MSR_AMD_PERFEVTSEL3);
                    flags |= (1<<22);  /* enable flag */
                    msr_write(cpu_id, MSR_AMD_PERFEVTSEL3, flags);
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

    timer_startCycles(&time[cpu_id]);
}

int
likwid_markerRegisterRegion(char* regionTag)
{
    static int lastRegion = -1;
    bstring tag = bfromcstr(regionTag);

    lastRegion++;

    if ( lastRegion >= likwid_numberOfRegions)
    {
        fprintf(stderr, "Too many regions %d %d\n",lastRegion,likwid_numberOfRegions);
        exit(EXIT_FAILURE);
    }

    results[lastRegion].tag = bstrcpy(tag);
    return lastRegion;
}

int
likwid_markerGetRegionId(char* regionTag)
{
    bstring tag = bfromcstr(regionTag);

    for (int i=0; i<likwid_numberOfRegions;i++)
    {
        if (biseq(results[i].tag,tag))
        {
            return i;
        }
    }
    fprintf(stderr, "WARNING: Region not found\n");

    return -1;
}


void
likwid_markerStopRegion(int cpu_id, int regionId)
{
    uint64_t flags;

    timer_stopCycles(&time[cpu_id]);

    switch ( cpuid_info.family ) 
    {
        case P6_FAMILY:

            msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);

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

                    results[regionId].cycles[cpu_id] += (double) msr_read(cpu_id, MSR_PERF_FIXED_CTR1);
                    results[regionId].instructions[cpu_id] += (double) msr_read(cpu_id, MSR_PERF_FIXED_CTR0);
                    results[regionId].counters[cpu_id][0] += (double) msr_read(cpu_id, MSR_PMC0);
                    results[regionId].counters[cpu_id][1] += (double) msr_read(cpu_id, MSR_PMC1);
                    break;

                case NEHALEM_BLOOMFIELD:

                case NEHALEM_LYNNFIELD:
                    results[regionId].cycles[cpu_id] += (double) msr_read(cpu_id, MSR_PERF_FIXED_CTR1);
                    results[regionId].instructions[cpu_id] += (double) msr_read(cpu_id, MSR_PERF_FIXED_CTR0);
                    results[regionId].counters[cpu_id][0] += (double) msr_read(cpu_id, MSR_PMC0);
                    results[regionId].counters[cpu_id][1] += (double) msr_read(cpu_id, MSR_PMC1);
                    results[regionId].counters[cpu_id][2] += (double) msr_read(cpu_id, MSR_PMC2);
                    results[regionId].counters[cpu_id][3] += (double) msr_read(cpu_id, MSR_PMC3);

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

            flags = msr_read(cpu_id,MSR_AMD_PERFEVTSEL0);
            flags &= ~(1<<22);  /* clear enable flag */
            msr_write(cpu_id, MSR_AMD_PERFEVTSEL0 , flags);
            flags = msr_read(cpu_id,MSR_AMD_PERFEVTSEL1);
            flags &= ~(1<<22);  /* clear enable flag */
            msr_write(cpu_id, MSR_AMD_PERFEVTSEL1 , flags);
            flags = msr_read(cpu_id,MSR_AMD_PERFEVTSEL2);
            flags &= ~(1<<22);  /* clear enable flag */
            msr_write(cpu_id, MSR_AMD_PERFEVTSEL2 , flags);
            flags = msr_read(cpu_id,MSR_AMD_PERFEVTSEL3);
            flags &= ~(1<<22);  /* clear enable flag */
            msr_write(cpu_id, MSR_AMD_PERFEVTSEL3 , flags);

            results[regionId].counters[cpu_id][0] += (double) msr_read(cpu_id, MSR_AMD_PMC0);
            results[regionId].counters[cpu_id][1] += (double) msr_read(cpu_id, MSR_AMD_PMC1);
            results[regionId].counters[cpu_id][2] += (double) msr_read(cpu_id, MSR_AMD_PMC2);
            results[regionId].counters[cpu_id][3] += (double) msr_read(cpu_id, MSR_AMD_PMC3);
            break;

        default:
            fprintf(stderr, "Unsupported Processor!\n");
            exit(EXIT_FAILURE);
            break;
    }

    results[regionId].time[cpu_id] += timer_printCyclesTime(&time[cpu_id]);
}


