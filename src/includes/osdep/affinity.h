#ifndef AFFINITY_H
#define AFFINITY_H

#include <stdint.h>

#include <osdep/affinitymask.h>

// pinning
extern int affinity_processGetProcessorId();
extern int affinity_threadGetProcessorId();

extern int affinity_pinProcess(int processorId);
extern int affinity_pinThread(int processorId);

// set affinity mask
extern int affinity_setProcessAffinityMask(AffinityMask affinityMask);
extern int affinity_setThreadAffinityMask(AffinityMask affinityMask);

/** Can be used to reset the process affinity mask.
  * Example:
  * If 2 processors are in the system, 0x3 = (0,1,1) will be returned.
  * If 3 processors are in the system, 0x7 = (1,1,1) will be returned.
  */
extern AffinityMask affinity_getLargestProcessAffinityMask();

#endif
