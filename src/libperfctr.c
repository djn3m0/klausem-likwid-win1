/*
 * ===========================================================================
 *
 *       Filename:  libperfctr.c
 *
 *    Description:  Library interface of module perfmon
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
perfmon_markerStartCounters(int cpu_id)
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

                case NEHALEM:

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
perfmon_markerStopCounters(int cpu_id)
{
    uint64_t flags;
    int i;

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

void perfmon_markerSetCycles(uint64_t cycles)
{
    FILE *file;

    file = fopen("/tmp/perfmon_cycles.txt","w");
    fprintf(file,"%llu",cycles);
    fclose(file);
}



