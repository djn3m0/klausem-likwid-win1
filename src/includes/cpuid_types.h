#ifndef CPUID_TYPES_H
#define CPUID_TYPES_H

typedef struct cpu_info {
    uint32_t family;
    uint32_t model;
    uint32_t stepping;
    uint64_t clock;
    char*  name;
    char*  features;
    uint32_t perf_version;
    uint32_t perf_num_ctr;
    uint32_t perf_width_ctr;
    uint32_t perf_num_fixed_ctr;
} CpuInfo;


#endif /*CPUID_TYPES_H*/
