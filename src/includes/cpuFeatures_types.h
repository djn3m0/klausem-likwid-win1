#ifndef CPUFEATURES_TYPES_H
#define CPUFEATURES_TYPES_H

typedef enum {
    HW_PREFETCHER=0,
    CL_PREFETCHER,
    DCU_PREFETCHER,
    IP_PREFETCHER} CpuFeature;

typedef struct {
	char fastStrings;
	char thermalControl;
	char perfMonitoring;
	char hardwarePrefetcher;
	char pebs;
	char speedstep;
	char clPrefetcher;
	char dcuPrefetcher;
	char dynamicAcceleration;
	char ipPrefetcher;
    } CpuFeatureFlags;



#endif /*CPUFEATURES_TYPES_H*/
