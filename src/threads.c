
/* #####   HEADER FILE INCLUDES   ######################################### */
#include <stdlib.h>
#include <stdio.h>

#include <types.h>
#include <threads.h>


/* #####   EXPORTED VARIABLES   ########################################### */

pthread_barrier_t threads_barrier;
ThreadData* threads_data;
ThreadGroup* threads_groups;

/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ###################### */

static pthread_t* threads = NULL;
static pthread_attr_t attr;
static int numThreads = 0;


/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ################## */

void
threads_init(int numberOfThreads)
{
    int i;
    numThreads = numberOfThreads;

    threads = (pthread_t*) malloc(numThreads * sizeof(pthread_t));
    threads_data = (ThreadData*) malloc(numThreads * sizeof(ThreadData));

    for(i = 0; i < numThreads; i++)
    {
        threads_data[i].numberOfThreads = numThreads;
        threads_data[i].globalNumberOfThreads = numThreads;
        threads_data[i].globalThreadId = i;
        threads_data[i].threadId = i;
    }

    pthread_barrier_init(&threads_barrier, NULL, numThreads);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
}


void 
threads_create(void *(*startRoutine)(void*))
{
    int i;

    for(i = 0; i < numThreads; i++)
    {
        pthread_create(&threads[i],
                &attr,
                startRoutine,
                (void*) &threads_data[i]); 
    }
}

void 
threads_createGroups(int numberOfGroups)
{
    int i;
    int j;
    int numThreadsPerGroup;
    int globalId = 0;

    if (numThreads % numberOfGroups)
    {
        fprintf(stderr, "THREADS ERROR: Impossible to create groups \n");
        exit(EXIT_FAILURE);
    }
    else 
    {
        numThreadsPerGroup = numThreads / numberOfGroups;
    }

    threads_groups = (ThreadGroup*) malloc(numberOfGroups *
            sizeof(ThreadGroup));

    for (i = 0; i < numberOfGroups; i++)
    {
        threads_groups[i].numberOfThreads = numThreadsPerGroup;
        threads_groups[i].threadIds = (int*) malloc(numThreadsPerGroup *
                sizeof(int));

        for (j = 0; j < numThreadsPerGroup; j++)
        {
            threads_data[globalId].threadId = j;
            threads_data[globalId].groupId = i;
            threads_data[globalId].numberOfGroups = numberOfGroups;
            threads_data[globalId].numberOfThreads = numThreadsPerGroup;
            threads_groups[i].threadIds[j] = globalId++;
        }
    }
}


void 
threads_registerDataAll(ThreadUserData* data, threads_copyDataFunc func)
{
    int i;

    if (func == NULL)
    {
        for(i = 0; i < numThreads; i++)
        {
            threads_data[i].data = (*data);
        }
    }
    else
    {
        for(i = 0; i < numThreads; i++)
        {
            func( data, &threads_data[i].data);
        }
    }
}

void
threads_registerDataThread(int threadId,
        ThreadUserData* data,
        threads_copyDataFunc func)
{
    if (func == NULL)
    {
        threads_data[threadId].data = (*data);
    }
    else
    {
        func( data, &threads_data[threadId].data);
    }
}

void
threads_registerDataGroup(int groupId,
        ThreadUserData* data,
        threads_copyDataFunc func)
{
    int i;

    if (func == NULL)
    {
        for (i = 0; i < threads_groups[groupId].numberOfThreads; i++)
        {
            threads_data[threads_groups[groupId].threadIds[i]].data = (*data);
        }
    }
    else
    {
        for (i = 0; i < threads_groups[groupId].numberOfThreads; i++)
        {
            func( data,
                    &threads_data[threads_groups[groupId].threadIds[i]].data);
        }
    }
}

void
threads_destroy(void)
{
    int i;

    for(i=0; i < numThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_attr_destroy(&attr);
}

