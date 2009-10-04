/*
 * ===========================================================================
 *
 *       Filename:  cpuFeatures.c
 *
 *    Description:  Implementation of cpuFeatures Module.
 *                  Provides an API to read out and print the IA32_MISC_ENABLE
 *                  model specific register on Intel x86 processors.
 *                  Allows to turn on and off the Hardware prefetcher
 *                  available.
 *
 *        Version:  1.0
 *        Created:  07/05/2009
 *       Revision:  none
 *
 *         Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *        Company:  RRZE Erlangen
 *        Project:  HPCUtil
 *      Copyright:  Copyright (c) 2009, Jan Treibig
 *
 * ===========================================================================
 */


/* #####   HEADER FILE INCLUDES   ######################################### */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <types.h>
#include <msr.h>
#include <textcolor.h>
#include <cpuFeatures.h>


CpuFeatureFlags cpuFeatureFlags;

/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ######################### */

#define MSR_IA32_MISC_ENABLE      0x1A0

#define PRINT_VALUE(color,string)  \
    color_on(BRIGHT,(color));      \
    printf(#string"\n");            \
    color_reset()

#define TEST_FLAG(feature,flag)  \
    if (flags & (1ULL<<(flag)))   \
    {                    \
		cpuFeatureFlags.feature = 1; \
    }                    \
    else                \
    {                \
		cpuFeatureFlags.feature = 0; \
    }


/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ################## */

void
cpuFeatures_init(int cpu)
{
    uint64_t flags = msr_read(cpu, MSR_IA32_MISC_ENABLE);

	TEST_FLAG(fastStrings,1);
	TEST_FLAG(thermalControl,3);
	TEST_FLAG(perfMonitoring,7);
	TEST_FLAG(hardwarePrefetcher,9);
	TEST_FLAG(pebs,12);
	TEST_FLAG(speedstep,16);
	TEST_FLAG(clPrefetcher,19);
	TEST_FLAG(dcuPrefetcher,37);
	TEST_FLAG(dynamicAcceleration,38);
	TEST_FLAG(ipPrefetcher,39);
}

void
cpuFeatures_print(int cpu)
{
    uint64_t flags = msr_read(cpu, MSR_IA32_MISC_ENABLE);

    printf(HLINE);
    printf("Fast-Strings: \t\t\t");
    if (flags & 1)
    {
        PRINT_VALUE(GREEN,enabled);
    }
    else
    {
        PRINT_VALUE(RED,disabled);
    }

    printf("Automatic Thermal Control: \t");
    if (flags & (1ULL<<3))
    {
        PRINT_VALUE(GREEN,enabled);
    }
    else
    {
        PRINT_VALUE(RED,disabled);
    }

    printf("Performance monitoring: \t");
    if (flags & (1ULL<<7))
    {
        PRINT_VALUE(GREEN,enabled);
    }
    else
    {
        PRINT_VALUE(RED,disabled);
    }

    printf("Hardware Prefetcher: \t\t");
    if (flags & (1ULL<<9)) 
    {
        PRINT_VALUE(RED,disabled);
    }
    else
    {
        PRINT_VALUE(GREEN,enabled);
    }

    printf("PEBS: \t\t\t\t");
    if (flags & (1ULL<<12)) 
    {
        PRINT_VALUE(RED,notsupported);
    }
    else
    {
        PRINT_VALUE(GREEN,supported);
    }

    printf("Intel Enhanced SpeedStep: \t");
    if (flags & (1ULL<<16)) 
    {
        PRINT_VALUE(GREEN,enabled);
    }
    else
    {
        PRINT_VALUE(RED,disabled);
    }

    printf("Adjacent Cache Line Prefetch: \t");
    if (flags & (1ULL<<19)) 
    {
        PRINT_VALUE(RED,disabled);
    }
    else
    {
        PRINT_VALUE(GREEN,enabled);
    }

    printf("DCU Prefetcher: \t\t");
    if (flags & (1ULL<<37)) 
    {
        PRINT_VALUE(RED,disabled);
    }
    else
    {
        PRINT_VALUE(GREEN,enabled);
    }

    printf("Intel Dynamic Acceleration: \t");
    if (flags & (1ULL<<38)) 
    {
        PRINT_VALUE(RED,disabled);
    }
    else 
    {
        PRINT_VALUE(GREEN,enabled);
    }

    printf("IP Prefetcher: \t\t\t");
    if (flags & (1ULL<<39)) 
    {
        PRINT_VALUE(RED,disabled);
    }
    else
    {
        PRINT_VALUE(GREEN,enabled);
    }
    printf(HLINE);
}

void 
cpuFeatures_enable(int cpu, CpuFeature type)
{
    uint64_t flags = msr_read(cpu, MSR_IA32_MISC_ENABLE);

    switch ( type )
    {
        case HW_PREFETCHER:
            printf("HW_PREFETCHER:\t");
            flags &= ~(1ULL<<9);
            break;

        case CL_PREFETCHER:
            printf("CL_PREFETCHER:\t");
            flags &= ~(1ULL<<19);
            break;

        case DCU_PREFETCHER:
            printf("DCU_PREFETCHER:\t");
            flags &= ~(1ULL<<37);
            break;

        case IP_PREFETCHER:
            printf("IP_PREFETCHER:\t");
            flags &= ~(1ULL<<39);
            break;

        default:
            printf("ERROR: CpuFeature not supported!\n");
            break;
    }
    PRINT_VALUE(GREEN,enabled);
    printf("\n");

    msr_write(cpu, MSR_IA32_MISC_ENABLE, flags);
}


void
cpuFeatures_disable(int cpu, CpuFeature type)
{
    uint64_t flags = msr_read(cpu, MSR_IA32_MISC_ENABLE);

    switch ( type ) 
    {
        case HW_PREFETCHER:
            printf("HW_PREFETCHER:\t");
            flags |= (1ULL<<9);
            break;

        case CL_PREFETCHER:
            printf("CL_PREFETCHER:\t");
            flags |= (1ULL<<19);
            break;

        case DCU_PREFETCHER:
            printf("DCU_PREFETCHER:\t");
            flags |= (1ULL<<37);
            break;

        case IP_PREFETCHER:
            printf("IP_PREFETCHER:\t");
            flags |= (1ULL<<39);
            break;

        default:
            printf("ERROR: CpuFeature not supported!\n");
            break;
    }
    PRINT_VALUE(RED,disabled);
    printf("\n");

    msr_write(cpu, MSR_IA32_MISC_ENABLE, flags);
}

