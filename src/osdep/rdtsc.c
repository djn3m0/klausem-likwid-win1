#ifdef WIN32

#include <osdep/rdtsc_asm.h>

void RDTSC_func(TscCounter *cpu_c) {
	LARGE_INTEGER lPerformanceCount;
	if (!QueryPerformanceCounter(&lPerformanceCount)) {
		perror("QueryPerformanceCounter");
		exit(1);
	}
	cpu_c->int32.lo = lPerformanceCount.LowPart;
	cpu_c->int32.hi = lPerformanceCount.HighPart;
}

#endif
