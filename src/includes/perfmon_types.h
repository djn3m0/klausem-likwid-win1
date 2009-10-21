#ifndef PERFMON_TYPES_H
#define PERFMON_TYPES_H


/* #####   EXPORTED TYPE DEFINITIONS   #################################### */

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
    FLOPS_DP,
    FLOPS_SP,
    L1,
    L2,
    L3,
    MEM,
    DATA,
    BRANCH,
    CPI,
    FRONTEND,
	CLUSTER,
	CLUSTER_FLOPS,
    TLB} PerfmonGroup;

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
    uint64_t instructionsRetired;
    PerfmonCounter counters[NUM_PMC];
    uint64_t pc[NUM_PMC];
} PerfmonThread;


#endif /*PERFMON_TYPES_H*/
