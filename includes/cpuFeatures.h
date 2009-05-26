#ifndef CPUFEATURES_H
#define CPUFEATURES_H

typedef enum cpuFeaturesEnum {
    HW_PREFETCHER=0,
    CL_PREFETCHER,
    DCU_PREFETCHER,
    IP_PREFETCHER} cpuFeatureType;

extern void cpuFeatures_print(void);
extern void cpuFeatures_enable(cpuFeatureType type);
extern void cpuFeatures_disable(cpuFeatureType type);

#endif /*CPUFEATURES_H*/
