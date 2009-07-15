#ifndef MSR_H
#define MSR_H

#include <types.h>

extern uint64_t readMSR(int cpu, uint32_t reg);
extern void writeMSR(int cpu, uint32_t reg, uint64_t data);

#endif /*MSR_H*/
