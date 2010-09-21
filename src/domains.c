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
#include <sched.h>
#include <time.h>

#include <types.h>
#include <domains.h>
#include <cpuid.h>
#include <tree.h>

/* #####   EXPORTED VARIABLES   ########################################### */


/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ######################### */

/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ###################### */

static int  numberOfDomains = 0;
static AffinityDomain*  domains;

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ########### */

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

void domains_init()
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


void domains_finalize()
{
    int i;

    for (i=0; i<numberOfDomains; i++) {
        free(domains[i].processorList);
    }
    free(domains);
}

const AffinityDomain* domains_getDomain(bstring domain)
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

void domains_printDomains()
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



