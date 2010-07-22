#ifndef RDTSC_H
#define RDTSC_H

#include <types.h>

#define RDTSC(cpu_c) RDTSC_func(&cpu_c)

void RDTSC_func(TscCounter *cpu_c);

#endif /*RDTSC_H*/
