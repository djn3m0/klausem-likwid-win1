#ifndef TYPES_H
#define TYPES_H

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

typedef unsigned char uint8 ;
typedef unsigned short uint16 ;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef struct cpu_info {
    uint32 family;
    uint32 model;
    uint32 stepping;
    uint64 clock;
    char*  name;
    char*  features;
    uint32 perf_version;
    uint32 perf_num_ctr;
    uint32 perf_width_ctr;
    uint32 perf_num_fixed_ctr;
} cpu_info;

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
    NUM_PMC} perfmon_counter_index_t;

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
    TLB} perfmon_group_t;

typedef enum perfmon_vendor {
    INTEL = 0,
    AMD,
    NUM_VENDORS} perfmon_vendors_t;

typedef enum perfmon_model {
    CORE2_ARCH = 0,
    NEHALEM_ARCH,
    NUM_MODELS} perfmon_model_t;

typedef struct perfmon_event {
    uint32 event_id;
    uint32 umask;
} perfmon_event_t;

typedef struct perfmon_hash_entry {
    char*  key;
    perfmon_event_t  event;
} perfmon_hash_entry_t;

typedef struct perfmon_counter {
    char*  label;
    int  init;
    uint64  config_reg;
    uint64  counter_reg;
} perfmon_counter_t;

typedef struct perfmon_thread {
    int cpu_id;
    uint64 cycles;
    perfmon_counter_t counters[NUM_PMC];
    uint64 pc[NUM_PMC];
} perfmon_thread_t;


#endif /*TYPES_H*/
