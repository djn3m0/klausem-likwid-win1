#ifndef CPUFEATURES_TYPES_H
#define CPUFEATURES_TYPES_H

typedef enum {
    HW_PREFETCHER=0,
    CL_PREFETCHER,
    DCU_PREFETCHER,
    IP_PREFETCHER} CpuFeature;

typedef struct {
	unsigned int fastStrings:1;
	unsigned int thermalControl:1;
	unsigned int perfMonitoring:1;
	unsigned int hardwarePrefetcher:1;
	unsigned int ferrMultiplex:1;
	unsigned int branchTraceStorage:1;
	unsigned int pebs:1;
	unsigned int speedstep:1;
	unsigned int monitor:1;
	unsigned int clPrefetcher:1;
	unsigned int speedstepLock:1;
	unsigned int cpuidMaxVal:1;
	unsigned int xdBit:1;
	unsigned int dcuPrefetcher:1;
	unsigned int dynamicAcceleration:1;
	unsigned int ipPrefetcher:1;
    } CpuFeatureFlags;



#endif /*CPUFEATURES_TYPES_H*/
