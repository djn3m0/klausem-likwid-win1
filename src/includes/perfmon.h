#ifndef PERFMON_H
#define PERFMON_H

#include <stdint.h>
#include <types.h>


extern void perfmon_init (int num_threads, int threads[]);
extern int  perfmon_setup_counter (PerfmonCounterIndex index, char* event);
extern void perfmon_setup_group(PerfmonGroup group);
extern void perfmon_start_all_counters (void);
extern void perfmon_stop_all_counters (void);
extern void perfmon_start_counters (int thread_id);
extern void perfmon_stop_counters (int thread_id);
extern void perfmon_print_results(void);
extern void perfmon_get_cycles(void);
extern void perfmon_set_cycles(uint64_t);


extern void perfmon_marker_start_counters(int core_id);
extern void perfmon_marker_stop_counters(int core_id);
extern void perfmon_marker_set_cycles(uint64_t cycles);

#endif /*PERFMON_H*/
