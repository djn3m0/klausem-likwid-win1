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


#endif /*STRUTIL_TYPES_H*/
