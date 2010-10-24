#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <string.h>
#include <sched.h>
#include <types.h>
#include <unistd.h>

#include <timer.h>
#include <threads.h>
#include <barrier.h>
#include <likwid.h>
#include <osdep/affinity.h>


//#define BARRIER pthread_barrier_wait(&threads_barrier)
#define BARRIER   barrier_synchronize(&barr)

#ifdef PERFMON
#define START_PERFMON likwid_markerStartRegion(threadId,pinning_threadGetProcessorId());
#define STOP_PERFMON  likwid_markerStopRegion(threadId,pinning_threadGetProcessorId(),0);
#else
#define START_PERFMON
#define STOP_PERFMON
#endif

#define EXECUTE(func)   \
    BARRIER; \
    if (data->globalThreadId == 0) \
    { \
        timer_startCycles(&time); \
    } \
    START_PERFMON  \
    for (i=0; i< (int) data->data.iter; i++) \
    {   \
    func; \
    } \
    BARRIER; \
    STOP_PERFMON  \
    if (data->globalThreadId == 0) \
    { \
        timer_stopCycles(&time); \
        data->cycles = timer_printCycles(&time); \
    } \
    BARRIER



void* runTest(void* arg)
{
    int threadId;
    int offset;
    int size;
    int i;
    BarrierData barr;
    ThreadData* data;
    ThreadUserData* myData;
    CyclesData time;
    FuncPrototype func;

    data = (ThreadData*) arg;
    myData = &(data->data);
    func = myData->test->kernel;
    threadId = data->threadId;
    barrier_registerThread(&barr, 0, data->globalThreadId);

    /* Prepare ptrs for thread */
    size = myData->size / data->numberOfThreads;
    size -= (size%myData->test->stride);
    offset = data->threadId * size;
    myData->size = size;

    switch ( myData->test->type )
    {
        case DataType_SINGLE:
            {
                float* sptr;
                for (i=0; i < (int) myData->test->streams; i++)
                {
                    sptr = (float*) myData->streams[i];
                    sptr +=  offset;
                    sptr +=  size;
                    myData->streams[i] = (float*) sptr;
                }
            }
            break;
        case DataType_DOUBLE:
            {
                double* dptr;
                for (i=0; i < (int) myData->test->streams; i++)
                {
                    dptr = (double*) myData->streams[i];
                    dptr +=  offset;
                    dptr +=  size;
                    myData->streams[i] = (double*) dptr;
                }
            }
            break;
    }

    /* pint the thread */
    affinity_pinThread(affinity_getCurrentThreadId(), myData->processors[threadId]);

    sleep(1);
    BARRIER;
    printf("Group: %d Thread %d Global Thread %d running on core %d - Vector length %d Offset %d\n",
            data->groupId,
            threadId,
            data->globalThreadId,
            affinity_threadGetProcessorId(affinity_getCurrentThreadId()),
            size,
            offset);
    BARRIER;

    switch ( myData->test->streams ) {
        case STREAM_1:
            EXECUTE(func(size,myData->streams[0]));
            break;
        case STREAM_2:
            EXECUTE(func(size,myData->streams[0],myData->streams[1]));
            break;
        case STREAM_3:
            EXECUTE(func(size,myData->streams[0],myData->streams[1],myData->streams[2]));
            break;
        case STREAM_4:
            EXECUTE(func(size,myData->streams[0],myData->streams[1],myData->streams[2],myData->streams[3]));
            break;
        case STREAM_5:
            EXECUTE(func(size,myData->streams[0],myData->streams[1],myData->streams[2],myData->streams[3],myData->streams[4]));
            break;
        case STREAM_6:
            EXECUTE(func(size,myData->streams[0],myData->streams[1],myData->streams[2],myData->streams[3],myData->streams[4],myData->streams[5]));
            break;
        case STREAM_7:
            EXECUTE(func(size,myData->streams[0],myData->streams[1],myData->streams[2],myData->streams[3],myData->streams[4],myData->streams[5],myData->streams[6]));
            break;
        case STREAM_8:
            EXECUTE(func(size,myData->streams[0],myData->streams[1],myData->streams[2],myData->streams[3],myData->streams[4],myData->streams[5],myData->streams[6],myData->streams[7]));
            break;
        case STREAM_9:
            EXECUTE(func(size,myData->streams[0],myData->streams[1],myData->streams[2],myData->streams[3],myData->streams[4],myData->streams[5],myData->streams[6],myData->streams[7],myData->streams[8]));
            break;
        case STREAM_10:
            EXECUTE(func(size,myData->streams[0],myData->streams[1],myData->streams[2],myData->streams[3],myData->streams[4],myData->streams[5],myData->streams[6],myData->streams[7],myData->streams[8],myData->streams[9]));
            break;
        default:
            break;
    }

    pthread_exit(NULL);
}


