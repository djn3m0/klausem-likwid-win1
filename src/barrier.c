#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include <types.h>
#include <barrier.h>

#define CACHELINE_SIZE 64

static BarrierGroup* groups;
static int currentGroupId = 0;
static int maxGroupId = 0;

int
barrier_registerGroup(int numThreads)
{
    int errorCode;

    if (currentGroupId > maxGroupId)
    {
        fprintf(stderr, "BARRIER ERROR Group ID too large\n");
        exit(EXIT_FAILURE);
    }

    groups[currentGroupId].numberOfThreads = numThreads;
    errorCode = posix_memalign(
            (void**) &groups[currentGroupId].groupBval,
            CACHELINE_SIZE, 
            numThreads * 32 * sizeof(int));

    if (errorCode)
    {
        if (errorCode == EINVAL) 
        {
            fprintf(stderr,
           "BARRIER ERROR: Alignment parameter is not a power of two\n");
            exit(EXIT_FAILURE);
        }
        if (errorCode == ENOMEM) 
        {
            fprintf(stderr,
           "BARRIER ERROR: Insufficient memory to fulfill the request\n");
            exit(EXIT_FAILURE);
        }
    }


    return currentGroupId++;
}

void
barrier_registerThread(BarrierData* barr, int groupId, int threadId)
{
    int errorCode;
    int i;
    int j = 1;
    if (groupId > currentGroupId)
    {
        fprintf(stderr, "BARRIER ERROR Group not yet registered \n");
        exit(EXIT_FAILURE);
    }
    if (threadId > groups[groupId].numberOfThreads)
    {
        fprintf(stderr, "BARRIER ERROR Thread ID %d too large for group %d\n",
                threadId, groupId);
        exit(EXIT_FAILURE);
    }

    barr->numberOfThreads = groups[groupId].numberOfThreads;
    barr->offset = 0;
    barr->val = 1;
    barr->bval =  groups[groupId].groupBval;
    errorCode = posix_memalign(
            (void**) &(barr->index),
            CACHELINE_SIZE, 
            barr->numberOfThreads * sizeof(int));

    if (errorCode)
    {
        if (errorCode == EINVAL) 
        {
            fprintf(stderr,
           "BARRIER ERROR: Alignment parameter is not a power of two\n");
            exit(EXIT_FAILURE);
        }
        if (errorCode == ENOMEM) 
        {
            fprintf(stderr,
           "BARRIER ERROR: Insufficient memory to fulfill the request\n");
            exit(EXIT_FAILURE);
        }
    }


    barr->index[0] = threadId;

    for (i = 0; i < barr->numberOfThreads; i++)
    {
        if (!(i == threadId))
        {
            barr->index[j++] = i;
        }
    }
}


void
barrier_init(int numberOfGroups) 
{ 
    maxGroupId = numberOfGroups-1;
    groups = (BarrierGroup*) malloc(numberOfGroups * sizeof(BarrierGroup));
}

void
barrier_synchronize(BarrierData* barr)
{
    int i;

    barr->bval[barr->index[0] * 32 +  barr->offset * 16] = barr->val;

    for (i = 1; i < barr->numberOfThreads; i++)
    {
        while (barr->bval[barr->index[i] * 32 + barr->offset * 16] != barr->val)
        {
            __asm__ ("pause");
        }
    } 
    
    if (barr->offset)
    {
        barr->val = !barr->val;
    }
    barr->offset = !barr->offset;
}



