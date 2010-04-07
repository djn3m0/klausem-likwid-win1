#ifndef STRUTIL_H
#define STRUTIL_H

#include <bstrlib.h>

typedef struct {
    bstring eventName;
    bstring counterName;
} StrUtilEvent;

typedef struct {
    StrUtilEvent* events;
    int numberOfEvents;
} StrUtilEventSet;

extern int str2int(const char* str);
extern int bstr_to_cpuset(int* threads,  bstring str);
extern void bstr_to_eventset(StrUtilEventSet* set, bstring str);
extern bstring bSecureInput (int maxlen, char* vgcCtx);
extern int bJustifyCenter (bstring b, int width);

#endif /*STRUTIL_H*/
