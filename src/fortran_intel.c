#include <types.h>
#include <perfmon.h>


void perfmon_marker_start_counters_(int* cpu_id)
{
    perfmon_marker_start_counters(*cpu_id);
}

void perfmon_marker_stop_counters_(int* cpu_id)
{
    perfmon_marker_stop_counters(*cpu_id);
}

