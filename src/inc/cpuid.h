#ifndef CPUID_H
#define CPUID_H
#include <types.h>

#define PENTIUM_M 0x0DU
#define CORE_DUO  0x0EU
#define CORE2_65  0x0FU
#define CORE2_45  0x17U
#define NEHALEM   0x1AU
#define XEON_MP   0x1DU
#define BARCELONA 0x02U
#define SHANGHAI  0x04U

#define  P6_FAMILY        0x6U
#define  NETBURST_FAMILY  0xFU
#define  K10_FAMILY  0x10U

extern cpu_info cpuid_info;

extern uint64 rdtsc (void);
extern void cpuid_init (void);

#endif /*CPUID_H*/
