#include <types.h>
#include <perfmon.h>
#include <rdtsc.h>


void perfmon_marker_start_counters_(int* cpu_id)
{
    perfmon_markerStartCounters(*cpu_id);
}

void perfmon_marker_stop_counters_(int* cpu_id)
{
    perfmon_markerStopCounters(*cpu_id);
}

void perfmon_marker_set_cycles_(uint64_t* ticks)
{
    perfmon_markerSetCycles(*ticks);
}

void rdtsc_(uint64_t* ts)
{
    TscCounter ticks;

    RDTSC(ticks);

    *ts = ticks.int64;
}


