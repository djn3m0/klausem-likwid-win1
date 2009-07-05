#ifndef MSR_H
#define MSR_H

#include <types.h>

extern uint64 readMSR(int cpu, uint32 reg);
extern void writeMSR(int cpu, uint32 reg, uint64 data);

#endif /*MSR_H*/
