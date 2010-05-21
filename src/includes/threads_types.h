#ifndef THREADS_TYPES_H
#define THREADS_TYPES_H

#include <stdint.h>

typedef struct {
    int        globalNumberOfThreads;
    int        numberOfThreads;
    int        globalThreadId;
    int        threadId;
    int        numberOfGroups;
    int        groupId;
    double      time;
    uint64_t   cycles;
    ThreadUserData data;
} ThreadData;

typedef struct {
    int        numberOfThreads;
    int*       threadIds;
} ThreadGroup;

typedef void (*threads_copyDataFunc)(ThreadUserData* src,ThreadUserData* dst);

#endif /*THREADS_TYPES_H*/
