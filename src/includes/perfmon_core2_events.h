#define NUM_ARCH_EVENTS_CORE2 387

static PerfmonEvent  core2_arch_events[NUM_ARCH_EVENTS_CORE2] = {
{"INSTR_RETIRED_ANY",
  "FIXC0", 
   0x00,0x00}
,{"CPU_CLK_UNHALTED_CORE",
  "FIXC1", 
   0x00,0x00}
,{"LOAD_BLOCK_STA",
  "PMC", 
   0x03,0x02}
,{"LOAD_BLOCK_STD",
  "PMC", 
   0x03,0x04}
,{"LOAD_BLOCK_OVERLAP_STORE",
  "PMC", 
   0x03,0x08}
,{"LOAD_BLOCK_UNTIL_RETIRE",
  "PMC", 
   0x03,0x10}
,{"LOAD_BLOCK_L1D",
  "PMC", 
   0x03,0x20}
,{"SB_DRAIN_CYCLES",
  "PMC", 
   0x04,0x01}
,{"STORE_BLOCK_ORDER",
  "PMC", 
   0x04,0x02}
,{"STORE_BLOCK_SNOOP",
  "PMC", 
   0x04,0x08}
,{"SEGMENT_REG_LOADS",
  "PMC", 
   0x06,0x00}
,{"SSE_PRE_NTA",
  "PMC", 
   0x07,0x00}
,{"SSE_PRE_L1",
  "PMC", 
   0x07,0x01}
,{"SSE_PRE_L2",
  "PMC", 
   0x07,0x02}
,{"SSE_PRE_STORES",
  "PMC", 
   0x07,0x03}
,{"DTLB_MISSES_ANY",
  "PMC", 
   0x08,0x01}
,{"DTLB_MISSES_MISS_LD",
  "PMC", 
   0x08,0x02}
,{"DTLB_MISSES_L0_MISS_LD",
  "PMC", 
   0x08,0x04}
,{"DTLB_MISSES_MISS_ST",
  "PMC", 
   0x08,0x08}
,{"MEMORY_DISAMBIGUATION_RESET",
  "PMC", 
   0x09,0x01}
,{"MEMORY_DISAMBIGUATION_SUCCESS",
  "PMC", 
   0x09,0x02}
,{"PAGE_WALK_COUNT",
  "PMC", 
   0x0C,0x01}
,{"PAGE_WALK_CYCLES",
  "PMC", 
   0x0C,0x02}
,{"FP_COMP_OPS_EXE",
  "PMC0", 
   0x10,0x00}
,{"FP_ASSIST",
  "PMC1", 
   0x11,0x00}
,{"MUL",
  "PMC1", 
   0x12,0x00}
,{"DIV",
  "PMC1", 
   0x13,0x00}
,{"CYCLES_DIV_BUSY",
  "PMC0", 
   0x14,0x00}
,{"IDLE_DURING_DIV",
  "PMC0", 
   0x18,0x00}
,{"DELAYED_BYPASS_FP",
  "PMC1", 
   0x19,0x00}
,{"DELAYED_BYPASS_SIMD",
  "PMC1", 
   0x19,0x01}
,{"DELAYED_BYPASS_LOAD",
  "PMC1", 
   0x19,0x02}
,{"L2_ADS_ALL_CORES",
  "PMC", 
   0x21,0xC0}
,{"L2_ADS_THIS_CORE",
  "PMC", 
   0x21,0x40}
,{"L2_DBUS_BUSY_RD_ALL_CORES",
  "PMC", 
   0x23,0xC0}
,{"L2_DBUS_BUSY_RD_THIS_CORE",
  "PMC", 
   0x23,0x40}
,{"L2_LINES_IN_ALL_CORES_ALL",
  "PMC", 
   0x24,0xF0}
,{"L2_LINES_IN_THIS_CORE_ALL",
  "PMC", 
   0x24,0x70}
,{"L2_LINES_IN_ALL_CORES_PREFETCH_ONLY",
  "PMC", 
   0x24,0xD0}
,{"L2_LINES_IN_THIS_CORE_PREFETCH_ONLY",
  "PMC", 
   0x24,0x50}
,{"L2_LINES_IN_ALL_CORES_NO_PREFETCH",
  "PMC", 
   0x24,0xC0}
,{"L2_LINES_IN_THIS_CORE_NO_PREFETCH",
  "PMC", 
   0x24,0x40}
,{"L2_M_LINES_IN_ALL_CORES",
  "PMC", 
   0x25,0xC0}
,{"L2_M_LINES_IN_THIS_CORE",
  "PMC", 
   0x25,0x40}
,{"L2_LINES_OUT_ALL_CORES_ALL",
  "PMC", 
   0x26,0xF0}
,{"L2_LINES_OUT_THIS_CORE_ALL",
  "PMC", 
   0x26,0x70}
,{"L2_LINES_OUT_ALL_CORES_PREFETCH_ONLY",
  "PMC", 
   0x26,0xD0}
,{"L2_LINES_OUT_THIS_CORE_PREFETCH_ONLY",
  "PMC", 
   0x26,0x50}
,{"L2_LINES_OUT_ALL_CORES_NO_PREFETCH",
  "PMC", 
   0x26,0xC0}
,{"L2_LINES_OUT_THIS_CORE_NO_PREFETCH",
  "PMC", 
   0x26,0x40}
,{"L2_M_LINES_OUT_ALL_CORES_ALL",
  "PMC", 
   0x27,0xF0}
,{"L2_M_LINES_OUT_THIS_CORE_ALL",
  "PMC", 
   0x27,0x70}
,{"L2_M_LINES_OUT_ALL_CORES_PREFETCH_ONLY",
  "PMC", 
   0x27,0xD0}
,{"L2_M_LINES_OUT_THIS_CORE_PREFETCH_ONLY",
  "PMC", 
   0x27,0x50}
,{"L2_M_LINES_OUT_ALL_CORES_NO_PREFETCH",
  "PMC", 
   0x27,0xC0}
,{"L2_M_LINES_OUT_THIS_CORE_NO_PREFETCH",
  "PMC", 
   0x27,0x40}
,{"L2_IFETCH_ALL_CORES_MODIFIED",
  "PMC", 
   0x28,0xC8}
,{"L2_IFETCH_ALL_CORES_EXCLUSIVE",
  "PMC", 
   0x28,0xC4}
,{"L2_IFETCH_ALL_CORES_SHARED",
  "PMC", 
   0x28,0xC2}
,{"L2_IFETCH_ALL_CORES_INVALID",
  "PMC", 
   0x28,0xC1}
,{"L2_IFETCH_ALL_CORES_MESI",
  "PMC", 
   0x28,0xCF}
,{"L2_IFETCH_THIS_CORE_MODIFIED",
  "PMC", 
   0x28,0x48}
,{"L2_IFETCH_THIS_CORE_EXCLUSIVE",
  "PMC", 
   0x28,0x44}
,{"L2_IFETCH_THIS_CORE_SHARED",
  "PMC", 
   0x28,0x42}
,{"L2_IFETCH_THIS_CORE_INVALID",
  "PMC", 
   0x28,0x41}
,{"L2_IFETCH_THIS_CORE_MESI",
  "PMC", 
   0x28,0x4F}
,{"L2_LD_ALL_CORES_ALL_MODIFIED",
  "PMC", 
   0x29,0xF8}
,{"L2_LD_ALL_CORES_ALL_EXCLUSIVE",
  "PMC", 
   0x29,0xF4}
,{"L2_LD_ALL_CORES_ALL_SHARED",
  "PMC", 
   0x29,0xF2}
,{"L2_LD_ALL_CORES_ALL_INVALID",
  "PMC", 
   0x29,0xF1}
,{"L2_LD_ALL_CORES_ALL_MESI",
  "PMC", 
   0x29,0xFF}
,{"L2_LD_THIS_CORE_ALL_MODIFIED",
  "PMC", 
   0x29,0x78}
,{"L2_LD_THIS_CORE_ALL_EXCLUSIVE",
  "PMC", 
   0x29,0x74}
,{"L2_LD_THIS_CORE_ALL_SHARED",
  "PMC", 
   0x29,0x72}
,{"L2_LD_THIS_CORE_ALL_INVALID",
  "PMC", 
   0x29,0x71}
,{"L2_LD_THIS_CORE_ALL_MESI",
  "PMC", 
   0x29,0x7F}
,{"L2_LD_ALL_CORES_PREFETCH_ONLY_MODIFIED",
  "PMC", 
   0x29,0xD8}
,{"L2_LD_ALL_CORES_PREFETCH_ONLY_EXCLUSIVE",
  "PMC", 
   0x29,0xD4}
,{"L2_LD_ALL_CORES_PREFETCH_ONLY_SHARED",
  "PMC", 
   0x29,0xD2}
,{"L2_LD_ALL_CORES_PREFETCH_ONLY_INVALID",
  "PMC", 
   0x29,0xD1}
,{"L2_LD_ALL_CORES_PREFETCH_ONLY_MESI",
  "PMC", 
   0x29,0xDF}
,{"L2_LD_THIS_CORE_PREFETCH_ONLY_MODIFIED",
  "PMC", 
   0x29,0x58}
,{"L2_LD_THIS_CORE_PREFETCH_ONLY_EXCLUSIVE",
  "PMC", 
   0x29,0x54}
,{"L2_LD_THIS_CORE_PREFETCH_ONLY_SHARED",
  "PMC", 
   0x29,0x52}
,{"L2_LD_THIS_CORE_PREFETCH_ONLY_INVALID",
  "PMC", 
   0x29,0x51}
,{"L2_LD_THIS_CORE_PREFETCH_ONLY_MESI",
  "PMC", 
   0x29,0x5F}
,{"L2_LD_ALL_CORES_NO_PREFETCH_MODIFIED",
  "PMC", 
   0x29,0xC8}
,{"L2_LD_ALL_CORES_NO_PREFETCH_EXCLUSIVE",
  "PMC", 
   0x29,0xC4}
,{"L2_LD_ALL_CORES_NO_PREFETCH_SHARED",
  "PMC", 
   0x29,0xC2}
,{"L2_LD_ALL_CORES_NO_PREFETCH_INVALID",
  "PMC", 
   0x29,0xC1}
,{"L2_LD_ALL_CORES_NO_PREFETCH_MESI",
  "PMC", 
   0x29,0xCF}
,{"L2_LD_THIS_CORE_NO_PREFETCH_MODIFIED",
  "PMC", 
   0x29,0x48}
,{"L2_LD_THIS_CORE_NO_PREFETCH_EXCLUSIVE",
  "PMC", 
   0x29,0x44}
,{"L2_LD_THIS_CORE_NO_PREFETCH_SHARED",
  "PMC", 
   0x29,0x42}
,{"L2_LD_THIS_CORE_NO_PREFETCH_INVALID",
  "PMC", 
   0x29,0x41}
,{"L2_LD_THIS_CORE_NO_PREFETCH_MESI",
  "PMC", 
   0x29,0x4F}
,{"L2_ST_ALL_CORES_MODIFIED",
  "PMC", 
   0x2A,0xC8}
,{"L2_ST_ALL_CORES_EXCLUSIVE",
  "PMC", 
   0x2A,0xC4}
,{"L2_ST_ALL_CORES_SHARED",
  "PMC", 
   0x2A,0xC2}
,{"L2_ST_ALL_CORES_INVALID",
  "PMC", 
   0x2A,0xC1}
,{"L2_ST_ALL_CORES_MESI",
  "PMC", 
   0x2A,0xCF}
,{"L2_ST_THIS_CORE_MODIFIED",
  "PMC", 
   0x2A,0x48}
,{"L2_ST_THIS_CORE_EXCLUSIVE",
  "PMC", 
   0x2A,0x44}
,{"L2_ST_THIS_CORE_SHARED",
  "PMC", 
   0x2A,0x42}
,{"L2_ST_THIS_CORE_INVALID",
  "PMC", 
   0x2A,0x41}
,{"L2_ST_THIS_CORE_MESI",
  "PMC", 
   0x2A,0x4F}
,{"L2_LOCK_ALL_CORES_MODIFIED",
  "PMC", 
   0x2B,0xC8}
,{"L2_LOCK_ALL_CORES_EXCLUSIVE",
  "PMC", 
   0x2B,0xC4}
,{"L2_LOCK_ALL_CORES_SHARED",
  "PMC", 
   0x2B,0xC2}
,{"L2_LOCK_ALL_CORES_INVALID",
  "PMC", 
   0x2B,0xC1}
,{"L2_LOCK_ALL_CORES_MESI",
  "PMC", 
   0x2B,0xCF}
,{"L2_LOCK_THIS_CORE_MODIFIED",
  "PMC", 
   0x2B,0x48}
,{"L2_LOCK_THIS_CORE_EXCLUSIVE",
  "PMC", 
   0x2B,0x44}
,{"L2_LOCK_THIS_CORE_SHARED",
  "PMC", 
   0x2B,0x42}
,{"L2_LOCK_THIS_CORE_INVALID",
  "PMC", 
   0x2B,0x41}
,{"L2_LOCK_THIS_CORE_MESI",
  "PMC", 
   0x2B,0x4F}
,{"L2_RQSTS_ALL_CORES_ALL_MODIFIED",
  "PMC", 
   0x2E,0xF8}
,{"L2_RQSTS_ALL_CORES_ALL_EXCLUSIVE",
  "PMC", 
   0x2E,0xF4}
,{"L2_RQSTS_ALL_CORES_ALL_SHARED",
  "PMC", 
   0x2E,0xF2}
,{"L2_RQSTS_ALL_CORES_ALL_INVALID",
  "PMC", 
   0x2E,0xF1}
,{"L2_RQSTS_ALL_CORES_ALL_MESI",
  "PMC", 
   0x2E,0xFF}
,{"L2_RQSTS_THIS_CORE_ALL_MODIFIED",
  "PMC", 
   0x2E,0x78}
,{"L2_RQSTS_THIS_CORE_ALL_EXCLUSIVE",
  "PMC", 
   0x2E,0x74}
,{"L2_RQSTS_THIS_CORE_ALL_SHARED",
  "PMC", 
   0x2E,0x72}
,{"L2_RQSTS_THIS_CORE_ALL_INVALID",
  "PMC", 
   0x2E,0x71}
,{"L2_RQSTS_THIS_CORE_ALL_MESI",
  "PMC", 
   0x2E,0x7F}
,{"L2_RQSTS_ALL_CORES_PREFETCH_ONLY_MODIFIED",
  "PMC", 
   0x2E,0xD8}
,{"L2_RQSTS_ALL_CORES_PREFETCH_ONLY_EXCLUSIVE",
  "PMC", 
   0x2E,0xD4}
,{"L2_RQSTS_ALL_CORES_PREFETCH_ONLY_SHARED",
  "PMC", 
   0x2E,0xD2}
,{"L2_RQSTS_ALL_CORES_PREFETCH_ONLY_INVALID",
  "PMC", 
   0x2E,0xD1}
,{"L2_RQSTS_ALL_CORES_PREFETCH_ONLY_MESI",
  "PMC", 
   0x2E,0xDF}
,{"L2_RQSTS_THIS_CORE_PREFETCH_ONLY_MODIFIED",
  "PMC", 
   0x2E,0x58}
,{"L2_RQSTS_THIS_CORE_PREFETCH_ONLY_EXCLUSIVE",
  "PMC", 
   0x2E,0x54}
,{"L2_RQSTS_THIS_CORE_PREFETCH_ONLY_SHARED",
  "PMC", 
   0x2E,0x52}
,{"L2_RQSTS_THIS_CORE_PREFETCH_ONLY_INVALID",
  "PMC", 
   0x2E,0x51}
,{"L2_RQSTS_THIS_CORE_PREFETCH_ONLY_MESI",
  "PMC", 
   0x2E,0x5F}
,{"L2_RQSTS_ALL_CORES_NO_PREFETCH_MODIFIED",
  "PMC", 
   0x2E,0xC8}
,{"L2_RQSTS_ALL_CORES_NO_PREFETCH_EXCLUSIVE",
  "PMC", 
   0x2E,0xC4}
,{"L2_RQSTS_ALL_CORES_NO_PREFETCH_SHARED",
  "PMC", 
   0x2E,0xC2}
,{"L2_RQSTS_ALL_CORES_NO_PREFETCH_INVALID",
  "PMC", 
   0x2E,0xC1}
,{"L2_RQSTS_ALL_CORES_NO_PREFETCH_MESI",
  "PMC", 
   0x2E,0xCF}
,{"L2_RQSTS_THIS_CORE_NO_PREFETCH_MODIFIED",
  "PMC", 
   0x2E,0x48}
,{"L2_RQSTS_THIS_CORE_NO_PREFETCH_EXCLUSIVE",
  "PMC", 
   0x2E,0x44}
,{"L2_RQSTS_THIS_CORE_NO_PREFETCH_SHARED",
  "PMC", 
   0x2E,0x42}
,{"L2_RQSTS_SELF_I_STATE",
  "PMC", 
   0x2E,0x41}
,{"L2_RQSTS_SELF_MESI",
  "PMC", 
   0x2E,0x4F}
,{"L2_REJECT_BUSQ_ALL_CORES_ALL_MODIFIED",
  "PMC", 
   0x30,0xF8}
,{"L2_REJECT_BUSQ_ALL_CORES_ALL_EXCLUSIVE",
  "PMC", 
   0x30,0xF4}
,{"L2_REJECT_BUSQ_ALL_CORES_ALL_SHARED",
  "PMC", 
   0x30,0xF2}
,{"L2_REJECT_BUSQ_ALL_CORES_ALL_INVALID",
  "PMC", 
   0x30,0xF1}
,{"L2_REJECT_BUSQ_ALL_CORES_ALL_MESI",
  "PMC", 
   0x30,0xFF}
,{"L2_REJECT_BUSQ_THIS_CORE_ALL_MODIFIED",
  "PMC", 
   0x30,0x78}
,{"L2_REJECT_BUSQ_THIS_CORE_ALL_EXCLUSIVE",
  "PMC", 
   0x30,0x74}
,{"L2_REJECT_BUSQ_THIS_CORE_ALL_SHARED",
  "PMC", 
   0x30,0x72}
,{"L2_REJECT_BUSQ_THIS_CORE_ALL_INVALID",
  "PMC", 
   0x30,0x71}
,{"L2_REJECT_BUSQ_THIS_CORE_ALL_MESI",
  "PMC", 
   0x30,0x7F}
,{"L2_REJECT_BUSQ_ALL_CORES_PREFETCH_ONLY_MODIFIED",
  "PMC", 
   0x30,0xD8}
,{"L2_REJECT_BUSQ_ALL_CORES_PREFETCH_ONLY_EXCLUSIVE",
  "PMC", 
   0x30,0xD4}
,{"L2_REJECT_BUSQ_ALL_CORES_PREFETCH_ONLY_SHARED",
  "PMC", 
   0x30,0xD2}
,{"L2_REJECT_BUSQ_ALL_CORES_PREFETCH_ONLY_INVALID",
  "PMC", 
   0x30,0xD1}
,{"L2_REJECT_BUSQ_ALL_CORES_PREFETCH_ONLY_MESI",
  "PMC", 
   0x30,0xDF}
,{"L2_REJECT_BUSQ_THIS_CORE_PREFETCH_ONLY_MODIFIED",
  "PMC", 
   0x30,0x58}
,{"L2_REJECT_BUSQ_THIS_CORE_PREFETCH_ONLY_EXCLUSIVE",
  "PMC", 
   0x30,0x54}
,{"L2_REJECT_BUSQ_THIS_CORE_PREFETCH_ONLY_SHARED",
  "PMC", 
   0x30,0x52}
,{"L2_REJECT_BUSQ_THIS_CORE_PREFETCH_ONLY_INVALID",
  "PMC", 
   0x30,0x51}
,{"L2_REJECT_BUSQ_THIS_CORE_PREFETCH_ONLY_MESI",
  "PMC", 
   0x30,0x5F}
,{"L2_REJECT_BUSQ_ALL_CORES_NO_PREFETCH_MODIFIED",
  "PMC", 
   0x30,0xC8}
,{"L2_REJECT_BUSQ_ALL_CORES_NO_PREFETCH_EXCLUSIVE",
  "PMC", 
   0x30,0xC4}
,{"L2_REJECT_BUSQ_ALL_CORES_NO_PREFETCH_SHARED",
  "PMC", 
   0x30,0xC2}
,{"L2_REJECT_BUSQ_ALL_CORES_NO_PREFETCH_INVALID",
  "PMC", 
   0x30,0xC1}
,{"L2_REJECT_BUSQ_ALL_CORES_NO_PREFETCH_MESI",
  "PMC", 
   0x30,0xCF}
,{"L2_REJECT_BUSQ_THIS_CORE_NO_PREFETCH_MODIFIED",
  "PMC", 
   0x30,0x48}
,{"L2_REJECT_BUSQ_THIS_CORE_NO_PREFETCH_EXCLUSIVE",
  "PMC", 
   0x30,0x44}
,{"L2_REJECT_BUSQ_THIS_CORE_NO_PREFETCH_SHARED",
  "PMC", 
   0x30,0x42}
,{"L2_REJECT_BUSQ_THIS_CORE_NO_PREFETCH_INVALID",
  "PMC", 
   0x30,0x41}
,{"L2_REJECT_BUSQ_THIS_CORE_NO_PREFETCH_MESI",
  "PMC", 
   0x30,0x4F}
,{"L2_NO_REQ_ALL_CORES",
  "PMC", 
   0x32,0xC0}
,{"L2_NO_REQ_THIS_CORE",
  "PMC", 
   0x32,0x40}
,{"EIST_TRANS",
  "PMC", 
   0x3A,0x00}
,{"THERMAL_TRIP",
  "PMC", 
   0x3B,0xC0}
,{"CPU_CLK_UNHALTED_CORE_P",
  "PMC", 
   0x3C,0x00}
,{"CPU_CLK_UNHALTED_BUS",
  "PMC", 
   0x3C,0x01}
,{"CPU_CLK_UNHALTED_NO_OTHER",
  "PMC", 
   0x3C,0x02}
,{"L1D_CACHE_LD_MODIFIED",
  "PMC", 
   0x40,0x08}
,{"L1D_CACHE_LD_EXCLUSIVE",
  "PMC", 
   0x40,0x04}
,{"L1D_CACHE_LD_SHARED",
  "PMC", 
   0x40,0x02}
,{"L1D_CACHE_LD_INVALID",
  "PMC", 
   0x40,0x01}
,{"L1D_CACHE_LD_MESI",
  "PMC", 
   0x40,0x0F}
,{"L1D_CACHE_ST_MODIFIED",
  "PMC", 
   0x41,0x08}
,{"L1D_CACHE_ST_EXCLUSIVE",
  "PMC", 
   0x41,0x04}
,{"L1D_CACHE_ST_SHARED",
  "PMC", 
   0x41,0x02}
,{"L1D_CACHE_ST_INVALID",
  "PMC", 
   0x41,0x01}
,{"L1D_CACHE_ST_MESI",
  "PMC", 
   0x41,0x0F}
,{"L1D_CACHE_LOCK_MODIFIED",
  "PMC", 
   0x42,0x08}
,{"L1D_CACHE_LOCK_EXCLUSIVE",
  "PMC", 
   0x42,0x04}
,{"L1D_CACHE_LOCK_SHARED",
  "PMC", 
   0x42,0x02}
,{"L1D_CACHE_LOCK_INVALID",
  "PMC", 
   0x42,0x01}
,{"L1D_CACHE_LOCK_MESI",
  "PMC", 
   0x42,0x0F}
,{"L1D_CACHE_LOCK_DURATION",
  "PMC", 
   0x42,0x10}
,{"L1D_ALL_REF",
  "PMC", 
   0x43,0x01}
,{"L1D_ALL_CACHE_REF",
  "PMC", 
   0x43,0x02}
,{"L1D_REPL",
  "PMC", 
   0x45,0x0F}
,{"L1D_M_REPL",
  "PMC", 
   0x46,0x00}
,{"L1D_M_EVICT",
  "PMC", 
   0x47,0x00}
,{"L1D_PEND_MISS",
  "PMC", 
   0x48,0x00}
,{"L1D_SPLIT_LOADS",
  "PMC", 
   0x49,0x01}
,{"L1D_SPLIT_STORES",
  "PMC", 
   0x49,0x02}
,{"SSE_PRE_MISS_NTA",
  "PMC", 
   0x4B,0x00}
,{"SSE_PRE_MISS_L1",
  "PMC", 
   0x4B,0x01}
,{"SSE_PRE_MISS_L2",
  "PMC", 
   0x4B,0x02}
,{"LOAD_HIT_PRE",
  "PMC", 
   0x4C,0x00}
,{"L1D_PREFETCH_REQUESTS",
  "PMC", 
   0x4E,0x10}
,{"BUS_REQUEST_OUTSTANDING_ALL_CORES_THIS_A",
  "PMC", 
   0x60,0xC0}
,{"BUS_REQUEST_OUTSTANDING_ALL_CORES_ALL_A",
  "PMC", 
   0x60,0xE0}
,{"BUS_REQUEST_OUTSTANDING_THIS_CORE_THIS_A",
  "PMC", 
   0x60,0x40}
,{"BUS_REQUEST_OUTSTANDING_THIS_CORE_ALL_A",
  "PMC", 
   0x60,0x60}
,{"BUS_BNR_DRV_THIS_A",
  "PMC", 
   0x61,0x00}
,{"BUS_BNR_DRV_ALL_A",
  "PMC", 
   0x61,0x20}
,{"BUS_DRDY_CLOCKS_THIS_A",
  "PMC", 
   0x62,0x00}
,{"BUS_DRDY_CLOCKS_ALL_A",
  "PMC", 
   0x62,0x20}
,{"BUS_LOCK_CLOCKS_ALL_CORES_THIS_A",
  "PMC", 
   0x63,0xC0}
,{"BUS_LOCK_CLOCKS_ALL_CORES_ALL_A",
  "PMC", 
   0x63,0xE0}
,{"BUS_LOCK_CLOCKS_THIS_CORE_THIS_A",
  "PMC", 
   0x63,0x40}
,{"BUS_LOCK_CLOCKS_THIS_CORE_ALL_A",
  "PMC", 
   0x63,0x60}
,{"BUS_DATA_RCV_ALL_CORES",
  "PMC", 
   0x64,0xC0}
,{"BUS_DATA_RCV_THIS_CORE",
  "PMC", 
   0x64,0x40}
,{"BUS_TRANS_BRD_ALL_CORES_THIS_A",
  "PMC", 
   0x65,0xC0}
,{"BUS_TRANS_BRD_ALL_CORES_ALL_A",
  "PMC", 
   0x65,0xE0}
,{"BUS_TRANS_BRD_THIS_CORE_THIS_A",
  "PMC", 
   0x65,0x40}
,{"BUS_TRANS_BRD_THIS_CORE_ALL_A",
  "PMC", 
   0x65,0x60}
,{"BUS_TRANS_RFO_ALL_CORES_THIS_A",
  "PMC", 
   0x66,0xC0}
,{"BUS_TRANS_RFO_ALL_CORES_ALL_A",
  "PMC", 
   0x66,0xE0}
,{"BUS_TRANS_RFO_THIS_CORE_THIS_A",
  "PMC", 
   0x66,0x40}
,{"BUS_TRANS_RFO_THIS_CORE_ALL_A",
  "PMC", 
   0x66,0x60}
,{"BUS_TRANS_WB_ALL_CORES_THIS_A",
  "PMC", 
   0x67,0xC0}
,{"BUS_TRANS_WB_ALL_CORES_ALL_A",
  "PMC", 
   0x67,0xE0}
,{"BUS_TRANS_WB_THIS_CORE_THIS_A",
  "PMC", 
   0x67,0x40}
,{"BUS_TRANS_WB_THIS_CORE_ALL_A",
  "PMC", 
   0x67,0x60}
,{"BUS_TRANS_IFETCH_ALL_CORES_THIS_A",
  "PMC", 
   0x68,0xC0}
,{"BUS_TRANS_IFETCH_ALL_CORES_ALL_A",
  "PMC", 
   0x68,0xE0}
,{"BUS_TRANS_IFETCH_THIS_CORE_THIS_A",
  "PMC", 
   0x68,0x40}
,{"BUS_TRANS_IFETCH_THIS_CORE_ALL_A",
  "PMC", 
   0x68,0x60}
,{"BUS_TRANS_INVAL_ALL_CORES_THIS_A",
  "PMC", 
   0x69,0xC0}
,{"BUS_TRANS_INVAL_ALL_CORES_ALL_A",
  "PMC", 
   0x69,0xE0}
,{"BUS_TRANS_INVAL_THIS_CORE_THIS_A",
  "PMC", 
   0x69,0x40}
,{"BUS_TRANS_INVAL_THIS_CORE_ALL_A",
  "PMC", 
   0x69,0x60}
,{"BUS_TRANS_PWR_ALL_CORES_THIS_A",
  "PMC", 
   0x6A,0xC0}
,{"BUS_TRANS_PWR_ALL_CORES_ALL_A",
  "PMC", 
   0x6A,0xE0}
,{"BUS_TRANS_PWR_THIS_CORE_THIS_A",
  "PMC", 
   0x6A,0x40}
,{"BUS_TRANS_PWR_THIS_CORE_ALL_A",
  "PMC", 
   0x6A,0x60}
,{"BUS_TRANS_P_ALL_CORES_THIS_A",
  "PMC", 
   0x6B,0xC0}
,{"BUS_TRANS_P_ALL_CORES_ALL_A",
  "PMC", 
   0x6B,0xE0}
,{"BUS_TRANS_P_THIS_CORE_THIS_A",
  "PMC", 
   0x6B,0x40}
,{"BUS_TRANS_P_THIS_CORE_ALL_A",
  "PMC", 
   0x6B,0x60}
,{"BUS_TRANS_IO_ALL_CORES_THIS_A",
  "PMC", 
   0x6C,0xC0}
,{"BUS_TRANS_IO_ALL_CORES_ALL_A",
  "PMC", 
   0x6C,0xE0}
,{"BUS_TRANS_IO_THIS_CORE_THIS_A",
  "PMC", 
   0x6C,0x40}
,{"BUS_TRANS_IO_THIS_CORE_ALL_A",
  "PMC", 
   0x6C,0x60}
,{"BUS_TRANS_DEF_ALL_CORES_THIS_A",
  "PMC", 
   0x6D,0xC0}
,{"BUS_TRANS_DEF_ALL_CORES_ALL_A",
  "PMC", 
   0x6D,0xE0}
,{"BUS_TRANS_DEF_THIS_CORE_THIS_A",
  "PMC", 
   0x6D,0x40}
,{"BUS_TRANS_DEF_THIS_CORE_ALL_A",
  "PMC", 
   0x6D,0x60}
,{"BUS_TRANS_BURST_ALL_CORES_THIS_A",
  "PMC", 
   0x6E,0xC0}
,{"BUS_TRANS_BURST_ALL_CORES_ALL_A",
  "PMC", 
   0x6E,0xE0}
,{"BUS_TRANS_BURST_THIS_CORE_THIS_A",
  "PMC", 
   0x6E,0x40}
,{"BUS_TRANS_BURST_THIS_CORE_ALL_A",
  "PMC", 
   0x6E,0x60}
,{"BUS_TRANS_MEM_ALL_CORES_THIS_A",
  "PMC", 
   0x6F,0xC0}
,{"BUS_TRANS_MEM_ALL_CORES_ALL_A",
  "PMC", 
   0x6F,0xE0}
,{"BUS_TRANS_MEM_THIS_CORE_THIS_A",
  "PMC", 
   0x6F,0x40}
,{"BUS_TRANS_MEM_THIS_CORE_ALL_A",
  "PMC", 
   0x6F,0x60}
,{"BUS_TRANS_ANY_ALL_CORES_THIS_A",
  "PMC", 
   0x70,0xC0}
,{"BUS_TRANS_ANY_ALL_CORES_ALL_A",
  "PMC", 
   0x70,0xE0}
,{"BUS_TRANS_ANY_THIS_CORE_THIS_A",
  "PMC", 
   0x70,0x40}
,{"BUS_TRANS_ANY_THIS_CORE_ALL_A",
  "PMC", 
   0x70,0x60}
,{"L1I_READS",
  "PMC", 
   0x80,0x00}
,{"L1I_MISSES",
  "PMC", 
   0x81,0x00}
,{"ITLB_SMALL_MISS",
  "PMC", 
   0x82,0x02}
,{"ITLB_LARGE_MISS",
  "PMC", 
   0x82,0x10}
,{"ITLB_FLUSH",
  "PMC", 
   0x82,0x40}
,{"ITLB_MISSES",
  "PMC", 
   0x82,0x12}
,{"INST_QUEUE_FULL",
  "PMC", 
   0x83,0x02}
,{"CYCLES_L1I_MEM_STALLED",
  "PMC", 
   0x86,0x00}
,{"ILD_STALL",
  "PMC", 
   0x87,0x00}
,{"BR_INST_EXEC",
  "PMC", 
   0x88,0x00}
,{"BR_MISSP_EXEC",
  "PMC", 
   0x89,0x00}
,{"BR_BAC_MISSP_EXEC",
  "PMC", 
   0x8A,0x00}
,{"BR_CND_EXEC",
  "PMC", 
   0x8B,0x00}
,{"BR_CND_MISSP_EXEC",
  "PMC", 
   0x8C,0x00}
,{"BR_IND_EXEC",
  "PMC", 
   0x8D,0x00}
,{"BR_IND_MISSP_EXEC",
  "PMC", 
   0x8E,0x00}
,{"BR_RET_EXEC",
  "PMC", 
   0x8F,0x00}
,{"BR_RET_BAC_MISSP_EXEC",
  "PMC", 
   0x91,0x00}
,{"BR_CALL_EXEC",
  "PMC", 
   0x92,0x00}
,{"BR_CALL_MISSP_EXEC",
  "PMC", 
   0x93,0x00}
,{"BR_IND_CALL_EXEC",
  "PMC", 
   0x94,0x00}
,{"BR_TKN_BUBBLE_1",
  "PMC", 
   0x97,0x00}
,{"BR_TKN_BUBBLE_2",
  "PMC", 
   0x98,0x00}
,{"RS_UOPS_DISPATCHED_ALL",
  "PMC", 
   0xA0,0x00}
,{"RS_UOPS_DISPATCHED_PORT0",
  "PMC0", 
   0xA1,0x01}
,{"RS_UOPS_DISPATCHED_PORT1",
  "PMC0", 
   0xA1,0x02}
,{"RS_UOPS_DISPATCHED_PORT2",
  "PMC0", 
   0xA1,0x04}
,{"RS_UOPS_DISPATCHED_PORT3",
  "PMC0", 
   0xA1,0x08}
,{"RS_UOPS_DISPATCHED_PORT4",
  "PMC0", 
   0xA1,0x10}
,{"RS_UOPS_DISPATCHED_PORT5",
  "PMC0", 
   0xA1,0x20}
,{"MACRO_INSTS_DECODED",
  "PMC", 
   0xAA,0x01}
,{"MACRO_INSTS_CISC_DECODED",
  "PMC", 
   0xAA,0x08}
,{"ESP_SYNCH",
  "PMC", 
   0xAB,0x01}
,{"ESP_ADDITIONS",
  "PMC", 
   0xAB,0x02}
,{"SIMD_UOPS_EXEC",
  "PMC", 
   0xB0,0x00}
,{"SIMD_SAT_UOPS_EXEC",
  "PMC", 
   0xB1,0x00}
,{"SIMD_UOP_TYPE_EXEC_MUL",
  "PMC", 
   0xB3,0x01}
,{"SIMD_UOP_TYPE_EXEC_SHIFT",
  "PMC", 
   0xB3,0x02}
,{"SIMD_UOP_TYPE_EXEC_PACK",
  "PMC", 
   0xB3,0x04}
,{"SIMD_UOP_TYPE_EXEC_UNPACK",
  "PMC", 
   0xB3,0x08}
,{"SIMD_UOP_TYPE_EXEC_LOGICAL",
  "PMC", 
   0xB3,0x10}
,{"SIMD_UOP_TYPE_EXEC_ARITHMETIC",
  "PMC", 
   0xB3,0x20}
,{"INST_RETIRED_ANY_P",
  "PMC", 
   0xC0,0x00}
,{"INST_RETIRED_LOADS",
  "PMC", 
   0xC0,0x01}
,{"INST_RETIRED_STORES",
  "PMC", 
   0xC0,0x02}
,{"INST_RETIRED_OTHER",
  "PMC", 
   0xC0,0x04}
,{"X87_OPS_RETIRED_FXCH",
  "PMC", 
   0xC1,0x01}
,{"X87_OPS_RETIRED_ANY",
  "PMC", 
   0xC1,0xFE}
,{"UOPS_RETIRED_LD_IND_BR",
  "PMC", 
   0xC2,0x01}
,{"UOPS_RETIRED_STD_STA",
  "PMC", 
   0xC2,0x02}
,{"UOPS_RETIRED_MACRO_FUSION",
  "PMC", 
   0xC2,0x04}
,{"UOPS_RETIRED_FUSED",
  "PMC", 
   0xC2,0x07}
,{"UOPS_RETIRED_NON_FUSED",
  "PMC", 
   0xC2,0x08}
,{"UOPS_RETIRED_ANY",
  "PMC", 
   0xC2,0x0F}
,{"MACHINE_NUKES_SMC",
  "PMC", 
   0xC3,0x01}
,{"MACHINE_NUKES_MEM_ORDER",
  "PMC", 
   0xC3,0x04}
,{"BR_INST_RETIRED_ANY",
  "PMC", 
   0xC4,0x00}
,{"BR_INST_RETIRED_PRED_NOT_TAKEN",
  "PMC", 
   0xC4,0x01}
,{"BR_INST_RETIRED_MISPRED_NOT_TAKEN",
  "PMC", 
   0xC4,0x02}
,{"BR_INST_RETIRED_PRED_TAKEN",
  "PMC", 
   0xC4,0x04}
,{"BR_INST_RETIRED_MISPRED_TAKEN",
  "PMC", 
   0xC4,0x08}
,{"BR_INST_RETIRED_TAKEN",
  "PMC", 
   0xC4,0x0C}
,{"BR_INST_RETIRED_MISPRED",
  "PMC", 
   0xC5,0x00}
,{"CYCLES_INT_MASKED",
  "PMC", 
   0xC6,0x01}
,{"CYCLES_INT_PENDING_MASKED",
  "PMC", 
   0xC6,0x02}
,{"SIMD_INST_RETIRED_PACKED_SINGLE",
  "PMC", 
   0xC7,0x01}
,{"SIMD_INST_RETIRED_SCALAR_SINGLE",
  "PMC", 
   0xC7,0x02}
,{"SIMD_INST_RETIRED_PACKED_DOUBLE",
  "PMC", 
   0xC7,0x04}
,{"SIMD_INST_RETIRED_SCALAR_DOUBLE",
  "PMC", 
   0xC7,0x08}
,{"SIMD_INST_RETIRED_VECTOR",
  "PMC", 
   0xC7,0x10}
,{"SIMD_INST_RETIRED_ANY",
  "PMC", 
   0xC7,0x1F}
,{"HW_INT_RCV",
  "PMC", 
   0xC8,0x00}
,{"ITLB_MISS_RETIRED",
  "PMC", 
   0xC9,0x00}
,{"SIMD_COMP_INST_RETIRED_PACKED_SINGLE",
  "PMC", 
   0xCA,0x01}
,{"SIMD_COMP_INST_RETIRED_SCALAR_SINGLE",
  "PMC", 
   0xCA,0x02}
,{"SIMD_COMP_INST_RETIRED_PACKED_DOUBLE",
  "PMC", 
   0xCA,0x04}
,{"SIMD_COMP_INST_RETIRED_SCALAR_DOUBLE",
  "PMC", 
   0xCA,0x08}
,{"MEM_LOAD_RETIRED_L1D_MISS",
  "PMC0", 
   0xCB,0x01}
,{"MEM_LOAD_RETIRED_L1D_LINE_MISS",
  "PMC0", 
   0xCB,0x02}
,{"MEM_LOAD_RETIRED_L2_MISS",
  "PMC0", 
   0xCB,0x04}
,{"MEM_LOAD_RETIRED_L2_LINE_MISS",
  "PMC0", 
   0xCB,0x08}
,{"MEM_LOAD_RETIRED_DTLB_MISS",
  "PMC0", 
   0xCB,0x10}
,{"FP_MMX_TRANS_TO_MMX",
  "PMC", 
   0xCC,0x01}
,{"FP_MMX_TRANS_TO_FP",
  "PMC", 
   0xCC,0x02}
,{"SIMD_ASSIST",
  "PMC", 
   0xCD,0x00}
,{"SIMD_INSTR_RETIRED",
  "PMC", 
   0xCE,0x00}
,{"SIMD_SAT_INSTR_RETIRED",
  "PMC", 
   0xCF,0x00}
,{"RAT_STALLS_ROB_READ_PORT",
  "PMC", 
   0xD2,0x01}
,{"RAT_STALLS_PARTIAL_CYCLES",
  "PMC", 
   0xD2,0x02}
,{"RAT_STALLS_FLAGS",
  "PMC", 
   0xD2,0x04}
,{"RAT_STALLS_FPSW",
  "PMC", 
   0xD2,0x08}
,{"RAT_STALLS_ANY",
  "PMC", 
   0xD2,0x0F}
,{"SEG_RENAME_STALLS_ES",
  "PMC", 
   0xD4,0x01}
,{"SEG_RENAME_STALLS_DS",
  "PMC", 
   0xD4,0x02}
,{"SEG_RENAME_STALLS_FS",
  "PMC", 
   0xD4,0x04}
,{"SEG_RENAME_STALLS_GS",
  "PMC", 
   0xD4,0x08}
,{"SEG_RENAME_STALLS_ANY",
  "PMC", 
   0xD4,0x0F}
,{"SEG_RENAME_STALLS_ES",
  "PMC", 
   0xD5,0x01}
,{"SEG_RENAME_STALLS_DS",
  "PMC", 
   0xD5,0x02}
,{"SEG_RENAME_STALLS_FS",
  "PMC", 
   0xD5,0x04}
,{"SEG_RENAME_STALLS_GS",
  "PMC", 
   0xD5,0x08}
,{"SEG_RENAME_STALLS_ANY",
  "PMC", 
   0xD5,0x0F}
,{"RESOURCE_STALLS_ROB_FULL",
  "PMC", 
   0xDC,0x01}
,{"RESOURCE_STALLS_RS_FULL",
  "PMC", 
   0xDC,0x02}
,{"RESOURCE_STALLS_LD_ST",
  "PMC", 
   0xDC,0x04}
,{"RESOURCE_STALLS_FPCW",
  "PMC", 
   0xDC,0x08}
,{"RESOURCE_STALLS_BR_MISS_CLEAR",
  "PMC", 
   0xDC,0x10}
,{"RESOURCE_STALLS_ANY",
  "PMC", 
   0xDC,0x1F}
,{"BR_INST_DECODED",
  "PMC", 
   0xE0,0x00}
,{"BOGUS_BR",
  "PMC", 
   0xE4,0x00}
,{"BACLEARS",
  "PMC", 
   0xE6,0x00}
,{"PREF_RQSTS_UP",
  "PMC", 
   0xF0,0x00}
,{"PREF_RQSTS_DN",
  "PMC", 
   0xF8,0x00}
};
