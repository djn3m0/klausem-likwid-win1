#ifndef MSR_H
#define MSR_H

#include <types.h>

extern uint64_t msr_read(int cpu, uint32_t reg);
extern void msr_write(int cpu, uint32_t reg, uint64_t data);

#endif /*MSR_H*/
