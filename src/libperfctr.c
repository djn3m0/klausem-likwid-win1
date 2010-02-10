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
#include <cpuid.h>
#include <msr.h>
#include <registers.h>
#include <perfmon.h>


/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ################## */

void
likwid_markerStartCounters(int cpu_id)
{
    uint64_t flags = 0x0ULL;
    cpuid_init();

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
}


void
likwid_markerStopCounters(int cpu_id)
{
    uint64_t flags;

    switch ( cpuid_info.family ) 
    {
        case P6_FAMILY:

            msr_write(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);

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

            break;

        default:
            fprintf(stderr, "Unsupported Processor!\n");
            exit(EXIT_FAILURE);
            break;
    }

}

void likwid_markerSetCycles(uint64_t cycles)
{
    FILE *file;

    file = fopen("/tmp/perfmon_cycles.txt","w");
    fprintf(file,"%llu", LLU_CAST cycles);
    fclose(file);
}



