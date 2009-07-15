#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

#define TRUE  1
#define FALSE 0

#define HLINE "-------------------------------------------------------------\n"
#define SLINE "*************************************************************\n"

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

typedef union
{  
    uint64_t int64;
    struct {uint32_t lo, hi;} int32;
} TscCounter;

typedef enum perfmon_index {
    PMC0 = 0,
    PMC1,
    PMC2,
    PMC3,
    PMCU0,
    PMCU1,
    PMCU2,
    PMCU3,
    PMCU4,
    PMCU5,
    PMCU6,
    PMCU7,
    NUM_PMC} PerfmonCounterIndex;

typedef enum perfmon_group {
    NOGROUP = 0,
    STD,
    FLOPS_SP,
    L1,
    L2,
    L3,
    MEM,
    DATA,
    BRANCH,
    CPI,
    TLB} PerfmonGroup;

typedef enum perfmon_vendor {
    INTEL = 0,
    AMD,
    NUM_VENDORS} PerfmonVendors;

typedef enum perfmon_model {
    CORE2_ARCH = 0,
    NEHALEM_ARCH,
    NUM_MODELS} PerfmonModel;

typedef enum cpuFeaturesEnum {
    HW_PREFETCHER=0,
    CL_PREFETCHER,
    DCU_PREFETCHER,
    IP_PREFETCHER} CpuFeature;

typedef struct perfmon_event {
    uint32_t event_id;
    uint32_t umask;
} PerfmonEvent;

typedef struct perfmon_hash_entry {
    char*  key;
    PerfmonEvent  event;
} PerfmonHashEntry;

typedef struct perfmon_counter {
    char*  label;
    int  init;
    uint64_t  config_reg;
    uint64_t  counter_reg;
} PerfmonCounter;

typedef struct perfmon_thread {
    int cpu_id;
    uint64_t cycles;
    PerfmonCounter counters[NUM_PMC];
    uint64_t pc[NUM_PMC];
} PerfmonThread;


#endif /*TYPES_H*/
