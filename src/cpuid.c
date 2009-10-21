/*
 * ===========================================================================
 *
 *       Filename:  cpuid.c
 *
 *    Description:  Implementation of cpuid module.
 *                  Provides API to extract cpuid info on x86 processors.
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

#include <timer.h>
#include <cpuid.h>
#include <tree.h>

/* #####   EXPORTED VARIABLES   ########################################### */

CpuInfo cpuid_info;
CpuTopology cpuid_topology;


/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ###################### */

static int largest_function;

/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ######################### */

/* this was taken from the linux kernel */
#define CPUID                              \
    asm volatile ("cpuid"                             \
        : "=a" (eax),     \
          "=b" (ebx),     \
          "=c" (ecx),     \
          "=d" (edx)      \
        : "0" (eax), "2" (ecx))


/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ###################### */

static char* pentium_m_b_str = "Intel Pentium M Banias processor";
static char* pentium_m_d_str = "Intel Pentium M Dothan processor";
static char* core_duo_str = "Intel Core Duo processor";
static char* core_2a_str = "Intel Core 2 65nm processor";
static char* core_2b_str = "Intel Core 2 45nm processor";
static char* nehalem_str = "Intel Core i7 processor";
static char* xeon_mp_string = "Intel Xeon MP processor";
static char* barcelona_str = "AMD Barcelona processor";
static char* shanghai_str = "AMD Shanghai processor";
static char* istanbul_str = "AMD Istanbul processor";
static char* opteron_dc_e_str = "AMD Opteron Dual Core Rev E 90nm processor";
static char* opteron_dc_f_str = "AMD Opteron Dual Core Rev F 90nm processor";
static char* athlon64_str = "AMD Athlon64 X2 (AM2) Rev F 90nm processor";
static char* athlon64_f_str = "AMD Athlon64 (AM2) Rev F 90nm processor";
static char* athlon64_X2_g_str = "AMD Athlon64 X2 (AM2) Rev G 65nm processor";
static char* athlon64_g_str = "AMD Athlon64 (AM2) Rev G 65nm processor";


static int lock = 0;
static uint32_t eax, ebx, ecx, edx;

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ########### */

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

static uint32_t
getBitFieldWidth(uint32_t number)
{
    uint32_t fieldWidth;

    number--;
    if (number == 0)
    {
        return 0;
    }

    asm volatile ( "bsr %%eax, %%ecx\n\t"
            : "=c" (fieldWidth)
            : "a"(number));


    return fieldWidth+1;  /* bsr returns the position, we want the width */
}

static uint32_t
amdGetAssociativity(uint32_t flag)
{
    uint32_t asso;

    switch ( flag ) 
    {
        case 0x0:
            asso = 0;
            break;

        case 0x1:
            asso = 1;
            break;

        case 0x2:
            asso = 2;
            break;

        case 0x4:
            asso = 4;
            break;

        case 0x6:
            asso = 8;
            break;

        case 0x8:
            asso = 16;
            break;

        case 0xA:
            asso = 32;
            break;

        case 0xB:
            asso = 48;
            break;

        case 0xC:
            asso = 64;
            break;

        case 0xD:
            asso = 96;
            break;

        case 0xE:
            asso = 128;
            break;

        case 0xF:
            asso = 0;
            break;

        default:
            break;
    }
    return asso;

}



/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ################## */

void
cpuid_init (void)
{
    if (lock) return;
    lock =1;

    eax = 0x00;
    CPUID;

    printf("CPUID Largest supported basic function 0x%X \n",eax);
    largest_function = eax;

    eax = 0x01;
    CPUID;
    cpuid_info.family = ((eax>>8)&0xFU) + ((eax>>20)&0xFFU);
    cpuid_info.model = (((eax>>16)&0xFU)<<4) + ((eax>>4)&0xFU);
    cpuid_info.stepping =  (eax&0xFU);
    cpuid_info.clock =  timer_getCpuClock();

    /*FIXME Add rejection of Netburst CPUs
     * Netburst is same family than K8
     */

    switch ( cpuid_info.family ) 
    {
        case P6_FAMILY:
            switch ( cpuid_info.model ) 
            {
                case PENTIUM_M_BANIAS:
                    cpuid_info.name = pentium_m_b_str;
                    break;

                case PENTIUM_M_DOTHAN:
                    cpuid_info.name = pentium_m_d_str;
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
                    fprintf(stderr, "Processor is not supported\n");
                    exit(EXIT_FAILURE);
                    break;
            }
            break;

        case K8_FAMILY:
            switch ( cpuid_info.model ) 
            {
                case OPTERON_DC_E:
                    cpuid_info.name = opteron_dc_e_str;
                    break;

                case OPTERON_DC_F:
                    cpuid_info.name = opteron_dc_f_str;
                    break;

                case ATHLON64_X2:

                case ATHLON64_X2_F:
                    cpuid_info.name = athlon64_str;
                    break;

                case ATHLON64_F1:

                case ATHLON64_F2:
                    cpuid_info.name = athlon64_f_str;
                    break;

                case ATHLON64_X2_G:
                    cpuid_info.name = athlon64_X2_g_str;
                    break;

                case ATHLON64_G1:

                case ATHLON64_G2:
                    cpuid_info.name = athlon64_g_str;
                    break;

                default:
                    fprintf(stderr, "Processor is not supported\n");
                    exit(EXIT_FAILURE);
                    break;
            }

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
                    fprintf(stderr, "Processor is not supported\n");
                    exit(EXIT_FAILURE);
                    break;
            }
            break;

        default:
			fprintf(stderr, "Processor is not supported\n");
			exit(EXIT_FAILURE);
            break;
    }

    cpuid_info.features = (char*) malloc(100*sizeof(char));
    if (edx & (1<<25)) strcpy(cpuid_info.features, "SSE ");
    if (edx & (1<<26)) strcat(cpuid_info.features, "SSE2 ");
    if (ecx & (1<<0))  strcat(cpuid_info.features, "SSE3 ");
    if (ecx & (1<<19)) strcat(cpuid_info.features, "SSE4.1 ");
    if (ecx & (1<<20)) strcat(cpuid_info.features, "SSE4.2 ");

    cpuid_info.perf_version   =  0;
    if( cpuid_info.family == P6_FAMILY && 0x0A <= largest_function) 
    {
        eax = 0x0A;
        CPUID;
        cpuid_info.perf_version   =  (eax&0xFFU);
        cpuid_info.perf_num_ctr   =   ((eax>>8)&0xFFU);
        cpuid_info.perf_width_ctr =  ((eax>>16)&0xFFU);
        cpuid_info.perf_num_fixed_ctr =  (edx&0xFU);
    }
}

void
cpuid_initTopology(void)
{
    uint32_t apicId;
    uint32_t bitField;
    int i;
    int level;
    int prevOffset = 0;
    int currOffset = 0;
    FILE* pipe;
    cpu_set_t set;
    HWThread* hwThreadPool;
    int hasBLeaf = 0;
    int maxNumLogicalProcs;
    int maxNumLogicalProcsPerCore;
    int maxNumCores;
    TreeNode* currentNode;
    int width;


    /* First determine the number of cpus accessible */
    pipe = popen("cat /proc/cpuinfo | grep ^processor | wc -l", "r");
    if (fscanf(pipe, "%d\n", &cpuid_topology.numHWThreads) != 1)
    {
        fprintf(stderr, "Failed to fscanf cpuinfo!\n");
        exit(EXIT_FAILURE);
    }

    if (cpuid_topology.numHWThreads == 1)
    {
        fprintf(stderr, "Single Core processor, exiting!\n");
        exit(EXIT_FAILURE);
    }

    hwThreadPool = (HWThread*) malloc(cpuid_topology.numHWThreads * sizeof(HWThread));

    /* check if 0x0B cpuid leaf is supported */
    eax = 0x0;
    CPUID;

    if (eax >= 0x0B)
    {
        eax = 0x0B;
        ecx = 0;
        CPUID;

        if (ebx) 
        {
            hasBLeaf = 1;
        }
    }

    tree_init(&cpuid_topology.topologyTree, 0);

    if (hasBLeaf)
    {
        for (i=0; i< cpuid_topology.numHWThreads; i++)
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

            eax = 0x0B;
            ecx = 0;
            CPUID;
            apicId = edx;
            hwThreadPool[i].apicId = apicId;

            for (level=0; level < 3; level++)
            {
                eax = 0x0B;
                ecx = level;
                CPUID;
                currOffset = eax&0xFU;

                switch ( level ) {
                    case 0:
                        bitField = extractBitField(apicId,
                                currOffset-prevOffset,
                                prevOffset);
                        hwThreadPool[i].threadId = bitField;
                        break;

                    case 1:
                        bitField = extractBitField(apicId,
                                currOffset-prevOffset,
                                prevOffset);
                        hwThreadPool[i].coreId = bitField;
                        break;

                    case 2:
                        bitField = extractBitField(apicId,
                                32-prevOffset,
                                prevOffset);
                        hwThreadPool[i].packageId = bitField;
                        break;

                }
                prevOffset = currOffset;
            }
        }
    }
    else
    {
        switch ( cpuid_info.family ) 
        {
            case P6_FAMILY:
                eax = 0x01;
                CPUID;
                /* Check number of cores per package */
                maxNumLogicalProcs = extractBitField(ebx,8,16);

                /* Check number of cores per package */
                eax = 0x04;
                ecx = 0;
                CPUID;
                maxNumCores = extractBitField(eax,6,26)+1;

                maxNumLogicalProcsPerCore = maxNumLogicalProcs/maxNumCores;

                for (i=0; i< cpuid_topology.numHWThreads; i++)
                {
                    CPU_ZERO(&set);
                    CPU_SET(i,&set);
                    if (sched_setaffinity(0, sizeof(cpu_set_t), &set))
                    {
                        if (errno == EFAULT) 
                        {
                            fprintf(stderr, 
                                    "A supplied memory address was invalid\n");
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

                    eax = 0x01;
                    CPUID;
                    hwThreadPool[i].apicId = extractBitField(ebx,8,24);

                    /* ThreadId is extracted from th apicId using the bit width
                     * of the number of logical processors
                     * */
                    hwThreadPool[i].threadId =
                        extractBitField(hwThreadPool[i].apicId,
                            getBitFieldWidth(maxNumLogicalProcsPerCore),0); 

                    /* CoreId is extracted from th apicId using the bitWidth 
                     * of the number of logical processors as offset and the
                     * bit width of the number of cores as width
                     * */
                    hwThreadPool[i].coreId =
                        extractBitField(hwThreadPool[i].apicId,
                            getBitFieldWidth(maxNumCores),
                            getBitFieldWidth(maxNumLogicalProcsPerCore)); 

                    hwThreadPool[i].packageId =
                        extractBitField(hwThreadPool[i].apicId,
                            8-getBitFieldWidth(maxNumLogicalProcs),
                            getBitFieldWidth(maxNumLogicalProcs)); 
                }
                break;

            case K10_FAMILY:
                eax = 0x80000008;
                CPUID;

                width =  extractBitField(ecx,4,12);

                if (width == 0)
                {
                    width =  extractBitField(ecx,8,0)+1;
                }

                eax = 0x01;
                CPUID;
                maxNumLogicalProcs =  extractBitField(ebx,8,16);
                maxNumCores = extractBitField(ecx,8,0)+1;


                for (i=0; i< cpuid_topology.numHWThreads; i++)
                {
                    CPU_ZERO(&set);
                    CPU_SET(i,&set);
                    if (sched_setaffinity(0, sizeof(cpu_set_t), &set))
                    {
                        if (errno == EFAULT) 
                        {
                            fprintf(stderr, 
                                    "A supplied memory address was invalid\n");
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

                    eax = 0x01;
                    CPUID;
                    hwThreadPool[i].apicId = extractBitField(ebx,8,24);
                    /* AMD only knows cores */
                    hwThreadPool[i].threadId = 0;

                    hwThreadPool[i].coreId =
                        extractBitField(hwThreadPool[i].apicId,
                                width, 0); 
                    hwThreadPool[i].packageId =
                        extractBitField(hwThreadPool[i].apicId,
                                (8-width), width); 
                }

                break;
        }
    }

    for (i=0; i< cpuid_topology.numHWThreads; i++)
    {
        /* Add node to Topology tree */
        if (!tree_nodeExists(cpuid_topology.topologyTree,
                    hwThreadPool[i].packageId))
        {
            tree_insertNode(cpuid_topology.topologyTree,
                    hwThreadPool[i].packageId);
        }
        currentNode = tree_getNode(cpuid_topology.topologyTree,
                hwThreadPool[i].packageId);

        if (!tree_nodeExists(currentNode, hwThreadPool[i].coreId))
        {
            tree_insertNode(currentNode, hwThreadPool[i].coreId);
        }
        currentNode = tree_getNode(currentNode, hwThreadPool[i].coreId);

        if (!tree_nodeExists(currentNode, i))
        {
            /*
               printf("WARNING: Thread already exists!\n");
               */
            tree_insertNode(currentNode, i);
        }

    }

    cpuid_topology.threadPool = hwThreadPool;

    cpuid_topology.numSockets = tree_countChildren(cpuid_topology.topologyTree);
    currentNode = tree_getNode(cpuid_topology.topologyTree, 0);
    cpuid_topology.numCoresPerSocket = tree_countChildren(currentNode);
    currentNode = tree_getNode(currentNode, 0);
    cpuid_topology.numThreadsPerCore = tree_countChildren(currentNode);
}

void 
cpuid_initCacheTopology()
{
    int i;
    int level=0;
    int maxNumLevels=0;
    uint32_t valid=1;
    CacheLevel* cachePool;

    switch ( cpuid_info.family ) 
    {
        case P6_FAMILY:

            while (valid)
            {
                eax = 0x04;
                ecx = level;
                CPUID;
                valid = extractBitField(eax,5,0);
                if (!valid)
                {
                    break;
                }
                level++;
            }

            maxNumLevels = level;
            cachePool = (CacheLevel*) malloc(maxNumLevels * sizeof(CacheLevel));

            for (i=0; i < maxNumLevels; i++) 
            {
                eax = 0x04;
                ecx = i;
                CPUID;

                cachePool[i].level = extractBitField(eax,3,5);
                cachePool[i].type = extractBitField(eax,5,0);
                cachePool[i].associativity = extractBitField(ebx,8,22)+1;
                cachePool[i].sets = ecx+1;
                cachePool[i].lineSize = extractBitField(ebx,12,0)+1;
                cachePool[i].size = cachePool[i].sets *
                    cachePool[i].associativity *
                    cachePool[i].lineSize;
                cachePool[i].threads = extractBitField(eax,10,14)+1;

 /* :WORKAROUND:08/13/2009 08:34:15 AM:jt: For L3 caches the value is sometimes 
  * too large in here. Ask Intel what is wrong here!
  * Limit threads per Socket than to the maximum possible value.*/
                if(cachePool[i].threads > 
                        (cpuid_topology.numCoresPerSocket*
                         cpuid_topology.numThreadsPerCore))
                {
                    cachePool[i].threads = cpuid_topology.numCoresPerSocket*
                         cpuid_topology.numThreadsPerCore;
                }
                cachePool[i].inclusive = edx&0x2;
            }

            break;

        case K10_FAMILY:
            /* FIXME: Adds one level for the instruction cache on Intel
             * This fixes the level for the cores
             */
            maxNumLevels = 3;
            cachePool = (CacheLevel*) malloc(maxNumLevels * sizeof(CacheLevel));

            eax = 0x80000005;
            CPUID;
            cachePool[0].level = 1;
            cachePool[0].type = DATACACHE;
            cachePool[0].associativity = extractBitField(ecx,8,16);
            cachePool[0].lineSize = extractBitField(ecx,8,0);
            cachePool[0].size =  extractBitField(ecx,8,24) * 1024;
            if ((cachePool[0].associativity * cachePool[0].lineSize) != 0)
            {
                cachePool[0].sets = cachePool[0].size/
                    (cachePool[0].associativity * cachePool[0].lineSize);
            }
            cachePool[0].threads = 1;
            cachePool[0].inclusive = 1;

            eax = 0x80000006;
            CPUID;
            cachePool[1].level = 2;
            cachePool[1].type = UNIFIEDCACHE;
            cachePool[1].associativity = 
                amdGetAssociativity(extractBitField(ecx,4,12));
            cachePool[1].lineSize = extractBitField(ecx,8,0);
            cachePool[1].size =  extractBitField(ecx,16,16) * 1024;
            if ((cachePool[0].associativity * cachePool[0].lineSize) != 0)
            {
                cachePool[1].sets = cachePool[1].size/
                    (cachePool[1].associativity * cachePool[1].lineSize);
            }
            cachePool[1].threads = 1;
            cachePool[1].inclusive = 1;

            cachePool[2].level = 3;
            cachePool[2].type = UNIFIEDCACHE;
            cachePool[2].associativity =
                amdGetAssociativity(extractBitField(edx,4,12));
            cachePool[2].lineSize = extractBitField(edx,8,0);
            cachePool[2].size =  (extractBitField(edx,14,18)+1) * 524288;
            if ((cachePool[0].associativity * cachePool[0].lineSize) != 0)
            {
                cachePool[2].sets = cachePool[1].size/
                    (cachePool[1].associativity * cachePool[1].lineSize);
            }
            cachePool[2].threads = cpuid_topology.numCoresPerSocket;
            cachePool[2].inclusive = 1;

            break;

    }

    cpuid_topology.numCacheLevels = maxNumLevels;
    cpuid_topology.cacheLevels = cachePool;
}



