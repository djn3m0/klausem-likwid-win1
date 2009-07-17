/*
 * ===========================================================================
 *
 *       Filename:  cpuid.c
 *
 *    Description:  Implementation of cpuid module.
 *                  Provides API to extract cpuid info on x86 processors.
 *                  Information is filled 
 *
 *        Version:  1.0
 *        Created:  07/06/2009
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

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>

#include <cpuid.h>

/* #####   EXPORTED VARIABLES   ########################################### */

CpuInfo cpuid_info;


/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ######################### */

#define CPUID(arg)                              \
    asm volatile ("movl $" #arg ", %%eax\n\t"       \
        "cpuid\n\t"                             \
        "movl %%eax, %0\n\t"                    \
        "movl %%ebx, %1\n\t"                    \
        "movl %%ecx, %2\n\t"                    \
        "movl %%edx, %3\n\t"                    \
        : "=r" (eax), "=r" (ebx), "=r" (ecx), "=r" (edx) \
        :                                                \
        : "%eax","%ebx","%ecx","%edx")

#define CPUID_TOPO(argA)                              \
        asm volatile ("movl $" #argA ", %%eax\n\t"                  \
            "cpuid\n\t"                             \
            "movl %%eax, %0\n\t"                   \
            "movl %%ebx, %1\n\t"                    \
            "movl %%ecx, %2\n\t"                   \
            "movl %%edx, %3\n\t"                  \
            : "=r" (eax), "=r" (ebx), "=c" (ecx), "=r" (edx) \
            : "c" (level)                                        \
            : "%eax","%ebx","%edx")


/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ###################### */

static char* pentium_m_str = "Intel Pentium M processor";
static char* core_duo_str = "Intel Core Duo processor";
static char* core_2a_str = "Intel Core 2 65nm processor";
static char* core_2b_str = "Intel Core 2 45nm processor";
static char* nehalem_str = "Intel Core i7 processor";
static char* xeon_mp_string = "Intel Xeon MP processor";
static char* barcelona_str = "AMD Barcelona processor";
static char* shanghai_str = "AMD Shanghai processor";
static char* istanbul_str = "AMD Istanbul processor";
static int lock = 0;
static uint32_t eax, ebx, ecx, edx;

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ########### */

static uint64_t
get_cpu_speed(void)
{
    uint64_t tsc1, tsc2;
    struct timeval tv1;
    struct timeval tv2;
    struct timezone tzp;
    struct timespec delay = { 0, 200000000 };  /* calibration time: 200 ms */

    tsc1 = rdtsc();
    gettimeofday( &tv1, &tzp);
    nanosleep( &delay, NULL);
    tsc2 = rdtsc();
    gettimeofday( &tv2, &tzp);

    return (tsc2 - tsc1) * 1000000 / 
        (((uint64_t)tv2.tv_sec * 1000000 + tv2.tv_usec) -
         ((uint64_t)tv1.tv_sec * 1000000 + tv1.tv_usec));
}

static uint32_t 
extractBitField(uint32_t inField, uint32_t width, uint32_t offset)
{
    uint32_t bitMask;
    uint32_t outField;

    if ((offset+width) == 32) 
    {
        bitMask = (0xFFFFFFFF<<offset);
    }
    else 
    {
        bitMask = (0xFFFFFFFF<<offset) ^ (0xFFFFFFFF<<(offset+width));

    }

    outField = (inField & bitMask) >> offset;
    return outField;
}


/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ################## */

void
cpuid_init (void)
{

    if (lock) return;
    lock =1;

    CPUID(0x01);
    cpuid_info.family = ((eax>>8)&0xFU) + ((eax>>20)&0xFFU);
    cpuid_info.model = (((eax>>16)&0xFU)<<4) + ((eax>>4)&0xFU);
    cpuid_info.stepping =  (eax&0xFU);
    cpuid_info.clock =  get_cpu_speed();

    switch ( cpuid_info.family ) 
    {
        case P6_FAMILY:
            switch ( cpuid_info.model ) 
            {
                case PENTIUM_M:
                    cpuid_info.name = pentium_m_str;
                    break;

                case CORE_DUO:
                    cpuid_info.name = core_duo_str;
                    break;

                case CORE2_65:
                    cpuid_info.name = core_2a_str;
                    break;

                case CORE2_45:
                    cpuid_info.name = core_2b_str;
                    break;

                case NEHALEM:
                    cpuid_info.name = nehalem_str;
                    break;

                case XEON_MP:
                    cpuid_info.name = xeon_mp_string;
                    break;

                default:
                    break;
            }
            break;

        case NETBURST_FAMILY:
            break;

        case K10_FAMILY:
            switch ( cpuid_info.model ) 
            {
                case BARCELONA:
                    cpuid_info.name = barcelona_str;
                    break;

                case SHANGHAI:
                    cpuid_info.name = shanghai_str;
                    break;

                case ISTANBUL:
                    cpuid_info.name = istanbul_str;
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    cpuid_info.features = (char*) malloc(100*sizeof(char));
    if (edx & (1<<25)) strcpy(cpuid_info.features, "SSE ");
    if (edx & (1<<26)) strcat(cpuid_info.features, "SSE2 ");
    if (ecx & (1<<0))  strcat(cpuid_info.features, "SSE3 ");
    if (ecx & (1<<19)) strcat(cpuid_info.features, "SSE4.1 ");
    if (ecx & (1<<20)) strcat(cpuid_info.features, "SSE4.2 ");

    if( cpuid_info.family == P6_FAMILY) 
    {
        CPUID(0x0A);
        cpuid_info.perf_version   =  (eax&0xFFU);
        cpuid_info.perf_num_ctr   =   ((eax>>8)&0xFFU);
        cpuid_info.perf_width_ctr =  ((eax>>16)&0xFFU);
        cpuid_info.perf_num_fixed_ctr =  (edx&0xFU);
    }
}

void
cpuid_topology(void)
{
    uint32_t apicId;
    uint32_t bitField;
    int numHWThreads = 0;
    int i;
    int level;
    int prevOffset = 0;
    int currOffset = 0;
    FILE* pipe;
    cpu_set_t set;
    HWThread* hwThreadPool;
    int hasBLeaf = 0;


    /* First determine the number of cpus accessible */
    pipe = popen("cat /proc/cpuinfo | grep processor | wc -l", "r");
    fscanf(pipe, "%d\n", &numHWThreads);
    hwThreadPool = (HWThread*) malloc(numHWThreads * sizeof(HWThread));

    printf("Number of Hardware Threads: %d \n",numHWThreads);


    /* check if 0x0B cpuid leaf is supported */
    CPUID(0x0);

    if (eax >= 0x0B)
    {
        level = 0;
        CPUID_TOPO(0x0B);

        if (ebx) 
        {
            hasBLeaf = 1;
        }
    }

    if (hasBLeaf)
    {
        for (i=0; i< numHWThreads; i++)
        {
            CPU_ZERO(&set);
            CPU_SET(i,&set);
            if (sched_setaffinity(0, sizeof(cpu_set_t), &set))
            {
                if (errno == EFAULT) 
                {
                    fprintf(stderr, "A supplied memory address was invalid\n");
                    exit(EXIT_FAILURE);
                }
                else if (errno == EINVAL) 
                {
                    fprintf(stderr, "Processor is not available\n");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    fprintf(stderr, "Unknown error\n");
                    exit(EXIT_FAILURE);
                }
            }

            level = 0;
            CPUID_TOPO(0x0B);
            apicId = edx;
            hwThreadPool[i].apicId = apicId;

            for (level=0; level < 3; level++)
            {
                CPUID_TOPO(0x0B);
                currOffset = eax&0xFU;
                
                switch ( level ) {
                    case 0:	
                        bitField = extractBitField(apicId,currOffset-prevOffset,prevOffset);
                        hwThreadPool[i].threadId = bitField;
                        break;

                    case 1:	
                        bitField = extractBitField(apicId,currOffset-prevOffset,prevOffset);
                        hwThreadPool[i].coreId = bitField;
                        break;

                    case 2:	
                        bitField = extractBitField(apicId,32-prevOffset,prevOffset);
                        hwThreadPool[i].packageId = bitField;
                        break;

                }
                prevOffset = currOffset;
            }
        }
    }
    else
    {
        printf("0x0B cpuid leaf not supported. Using legacy mode.\n");
        /* Check if SMT is supported */
        CPUID(0x01);
        if (edx & (1<<28))
        {
            printf("Hyperthreading is supported!.\n");
        }
        else
        {
            printf("Hyperthreading is not supported!.\n");
        }

        /* Check number of cores per package */
        printf("Logical Threads in Package: %u \n",extractBitField(ebx,8,16));

        /* Check number of cores per package */
        level = 0;
        CPUID_TOPO(0x04);
        printf("Logical Cores in Package: %u \n",extractBitField(eax,6,26)+1);

        for (i=0; i< numHWThreads; i++)
        {
            CPU_ZERO(&set);
            CPU_SET(i,&set);
            if (sched_setaffinity(0, sizeof(cpu_set_t), &set))
            {
                if (errno == EFAULT) 
                {
                    fprintf(stderr, "A supplied memory address was invalid\n");
                    exit(EXIT_FAILURE);
                }
                else if (errno == EINVAL) 
                {
                    fprintf(stderr, "Processor is not available\n");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    fprintf(stderr, "Unknown error\n");
                    exit(EXIT_FAILURE);
                }
            }

    }

    printf("HWThread\tThreadID\tCoreID\tPackageID\n");
    for (i=0; i< numHWThreads; i++)
    {
        printf("%d\t\t%d\t\t%d\t\t%d\n",i
                ,hwThreadPool[i].threadId
                ,hwThreadPool[i].coreId
                ,hwThreadPool[i].packageId);
    }
    
}

