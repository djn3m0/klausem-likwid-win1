#ifndef STRUTIL_H
#define STRUTIL_H

extern int str2int(const char* str);
extern int cstr_to_cpuset(int* threads,  const char* str);
extern void cstr_to_eventset(PerfmonEventSet* set, const char* str);

#endif /*STRUTIL_H*/
