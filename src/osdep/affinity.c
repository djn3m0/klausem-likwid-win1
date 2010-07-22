/*
 * ===========================================================================
 *
 *      Filename:  affinity.c
 *
 *      Description:  Implementation of affinity module.
 *
 *      Version:  <VERSION>
 *      Created:  <DATE>
 *
 *      Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *      Company:  RRZE Erlangen
 *      Project:  likwid
 *      Copyright:  Copyright (c) 2010, Jan Treibig
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
#ifndef WIN32
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <sched.h>
#include <time.h>
#include <pthread.h>

#include <types.h>
#include <affinity.h>
#include <cpuid.h>
#include <tree.h>

/* #####   EXPORTED VARIABLES   ########################################### */


/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ######################### */

#define gettid() syscall(SYS_gettid)


/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ###################### */

static int  numberOfDomains = 0;
static AffinityDomain*  domains;

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ########### */

#ifdef WIN32
static int
getProcessorID(DWORD_PTR* processAffinityMask)
{
    int processorId;

    for (processorId=0;processorId<24;processorId++){
        if (*processAffinityMask & (1 << processorId))
        {
            return processorId;
        }
    }
    fprintf(stderr, "Could not determine processor id from process affinity mask\n");
	return 25;
}
#else
static int
getProcessorID(cpu_set_t* processAffinityMask)
{
    int processorId;

    for (processorId=0;processorId<24;processorId++){
        if (CPU_ISSET(processorId,processAffinityMask))
        {
            return processorId;
        }
    }
    fprintf(stderr, "Could not determine processor id from process affinity mask\n");
	return 25;
}
#endif

static void
treeFillNextEntries(int numberOfEntries, int* processorIds, TreeNode** tree)
{
    int counter = numberOfEntries;
    TreeNode* threadNode;

    while (tree != NULL)
    {
        if (!counter) return;
        threadNode = tree_getChildNode(*tree);
        processorIds[numberOfEntries-counter] = threadNode->id;
        *tree = tree_getNextNode(*tree);
        counter--;
    }
}


/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ################## */

void affinity_init()
{
    int i;
    int cacheDomain;
    int currentDomain;
	int numberOfCoresPerCache;
    TreeNode* socketNode;
    TreeNode* coreNode;

    cpuid_init();
    cpuid_initTopology();
    cpuid_initCacheTopology();

    numberOfCoresPerCache =
        cpuid_topology.cacheLevels[cpuid_topology.numCacheLevels-1].threads/cpuid_topology.numThreadsPerCore;

    /* determine total number of domains */
    numberOfDomains = cpuid_topology.numSockets;

    /* for the cache domain take only into account last level cache and assume
     * all sockets to be uniform.
     * */

    /* determine how many last level shared caches exist per socket */
 //   numberOfDomains += cpuid_topology.numSockets * (cpuid_topology.numCoresPerSocket/numberOfCoresPerCache);
    numberOfDomains += cpuid_topology.numSockets * (cpuid_topology.numCoresPerSocket/numberOfCoresPerCache);

     /* :TODO:05/01/2010 10:02:36 AM:jt: Add NUMA domains here */

    domains = (AffinityDomain*) malloc(numberOfDomains * sizeof(AffinityDomain));
    cacheDomain = 0;

    for (i=0; i<numberOfDomains; i++) {
        if (i < (int) cpuid_topology.numSockets)
        {
            domains[i].numberOfProcessors = cpuid_topology.numCoresPerSocket;
            domains[i].processorList = (int*) malloc(cpuid_topology.numCoresPerSocket*sizeof(int));
            domains[i].tag = bformat("S%d",i);
        }
        else
        {
            domains[i].processorList = (int*)
                malloc(numberOfCoresPerCache*sizeof(int));
            domains[i].tag = bformat("C%d",cacheDomain++);
            domains[i].numberOfProcessors = numberOfCoresPerCache;
        }
    }

    currentDomain = -1;

    /* create socket domains considering only cores*/
    socketNode = tree_getChildNode(cpuid_topology.topologyTree);
    while (socketNode != NULL)
    {
        currentDomain++;
        coreNode = tree_getChildNode(socketNode);
        treeFillNextEntries(domains[currentDomain].numberOfProcessors, domains[currentDomain].processorList, &coreNode);
        socketNode = tree_getNextNode(socketNode);
    }

    /* create last level cache domains considering only cores */
    socketNode = tree_getChildNode(cpuid_topology.topologyTree);
    while (socketNode != NULL)
    {
        coreNode = tree_getChildNode(socketNode);
        for (i=0; i< (int) (cpuid_topology.numCoresPerSocket/numberOfCoresPerCache); i++)
        {
            currentDomain++;
            treeFillNextEntries(domains[currentDomain].numberOfProcessors, domains[currentDomain].processorList, &coreNode);
        }

        socketNode = tree_getNextNode(socketNode);
    }
}


void affinity_finalize()
{
    int i;

    for (i=0; i<numberOfDomains; i++) {
        free(domains[i].processorList);
    }
    free(domains);
}


int  affinity_processGetProcessorId()
{
#ifdef WIN32
	DWORD_PTR processAffinityMask;
	DWORD_PTR systemAffinityMask;

	BOOL res = GetProcessAffinityMask(
	  GetCurrentProcess(),
	  &processAffinityMask,
	  &systemAffinityMask
	);
	if (res == 0) {
		perror("GetCurrentProcess");
		exit(1);
	}
	return getProcessorID(&processAffinityMask);
#else
    cpu_set_t processAffinityMask;
    CPU_ZERO(&processAffinityMask);
    sched_getaffinity(getpid(),sizeof(cpu_set_t), &processAffinityMask);

    return getProcessorID(&processAffinityMask);
#endif
}


int  affinity_threadGetProcessorId()
{
#ifdef WIN32
	DWORD_PTR
		tmpThreadAffinityMask = (1 << 0),
		tmpThreadAffinityMask1 = -1;

	// temporarily set the thread affinity mask and retrieve old affinity mask
	DWORD_PTR origThreadAffinityMask = SetThreadAffinityMask(
		GetCurrentThread(),
		tmpThreadAffinityMask
	);
	if (origThreadAffinityMask == 0) {
		perror("SetThreadAffinityMask, used to get the thread affinity mask (1)");
		exit(1);
	}
	// reset the thread affinity mask to its original value
	tmpThreadAffinityMask1 = SetThreadAffinityMask(
		GetCurrentThread(),
		origThreadAffinityMask
	);
	if (tmpThreadAffinityMask1 == 0) {
		perror("SetThreadAffinityMask, used to get the thread affinity mask (2)");
		exit(1);
	}
	// check whether the resetting succeeded
	if (tmpThreadAffinityMask1 != tmpThreadAffinityMask) {
		perror("SetThreadAffinityMask, failed to reset thread affinity mask");
		exit(1);
	}
	return getProcessorID(&origThreadAffinityMask);
#else
    cpu_set_t  cpu_set;
    CPU_ZERO(&cpu_set);
    sched_getaffinity(gettid(),sizeof(cpu_set_t), &cpu_set);

    return getProcessorID(&cpu_set);
#endif
}


int  affinity_pinThread(int processorId)
{
#ifdef WIN32
	DWORD_PTR origThreadAffinityMask = SetThreadAffinityMask(
		GetCurrentThread(),
		1 << processorId
	);
	if (origThreadAffinityMask == 0) {
		perror("SetThreadAffinityMask");
		return FALSE;
	}
	return TRUE;
#else
	cpu_set_t cpuset;
    pthread_t thread;

    thread = pthread_self();
    CPU_ZERO(&cpuset);
    CPU_SET(processorId, &cpuset);
    if (pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset)) 
    {
        perror("pthread_setaffinity_np failed");
        return FALSE;
    }

    return TRUE;
#endif
}


int  affinity_pinProcess(int processorId)
{
#ifdef WIN32
	BOOL r = SetProcessAffinityMask(
		GetCurrentProcess(),
		1 << processorId
	);
	if (r == 0) {
		perror("SetProcessAffinityMask");
		return FALSE;
	}
	return TRUE;
#else
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(processorId, &cpuset);
	if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) == -1) 
	{
		perror("sched_setaffinity failed");
		/*
		TODO: take this error treatment or the one above?
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
		*/
        return FALSE;
	}

    return TRUE;
#endif
}


const AffinityDomain* affinity_getDomain(bstring domain)
{
    int i;

    for (i=0; i<numberOfDomains; i++)
    {
        if (biseq(domain, domains[i].tag))
        {
            return domains+i;
        }
    }

    return NULL;
}

void affinity_printDomains()
{
    int i;
    int j;

    for (i=0; i<numberOfDomains; i++)
    {
        printf("Domain %d:\n",i);
        printf("\tTag %s:",bdata(domains[i].tag));
        for (j=0; j<domains[i].numberOfProcessors; j++)
        {
            printf(" %d",domains[i].processorList[j]);
        }
        printf("\n");
    }
}



