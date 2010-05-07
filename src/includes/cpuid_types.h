#ifndef CPUID_TYPES_H
#define CPUID_TYPES_H

typedef enum {
    DATACACHE=1,
    INSTRUCTIONCACHE,
    UNIFIEDCACHE} CacheType;

typedef enum {
    NODE=0,
    SOCKET,
    CORE,
    THREAD} NodeLevel;

typedef struct {
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

typedef struct {
    uint32_t threadId;
    uint32_t coreId;
    uint32_t packageId;
    uint32_t apicId;
} HWThread;

typedef struct {
    int level;
    CacheType type;
    int associativity;
    int sets;
    int lineSize;
    int size;
    int threads;
    int inclusive;
} CacheLevel;

typedef struct {
    uint32_t numHWThreads;
    uint32_t numSockets;
    uint32_t numCoresPerSocket;
    uint32_t numThreadsPerCore;
    uint32_t numCacheLevels;
    HWThread* threadPool;
    CacheLevel*  cacheLevels;
    TreeNode* topologyTree;
} CpuTopology;


#endif /*CPUID_TYPES_H*/
