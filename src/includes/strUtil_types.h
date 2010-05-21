#ifndef STRUTIL_TYPES_H
#define STRUTIL_TYPES_H

#include  <bstrlib.h>


typedef struct {
    bstring eventName;
    bstring counterName;
} StrUtilEvent;

typedef struct {
    StrUtilEvent* events;
    int numberOfEvents;
} StrUtilEventSet;

typedef struct {
    bstring domain;
    int offset;
    void* ptr;
} Stream;

typedef struct {
    int numberOfThreads;
    int* processorIds;
    int size;
    Stream* streams;
} Workgroup;


#endif /*STRUTIL_TYPES_H*/
