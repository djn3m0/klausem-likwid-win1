#include <stdlib.h>
#include <stdio.h>

#include <timer.h>

int baseCycles=0;
float cpuclock=0.0F;
static uint64_t cycle_overhead;

inline uint64_t rdtsc(void)
{
   unsigned int tsc_low, tsc_high;

   asm( "rdtsc\n\t"
	"movl %%eax, %0\n\t"
	"movl %%edx, %1\n\t"
        : "=r" (tsc_low), "=r" (tsc_high)
        :
        : "%eax", "%edx");      

   return (uint64_t)tsc_high << 32 | (uint64_t)tsc_low;
}


void initTimer(void )
{
    uint64_t start,stop;
#if 0
    baseCycles = cyclesForCpuId();
    cpuclock = cpuClockFrequency();
#endif

    start = rdtsc();
    stop = rdtsc();
    cycle_overhead = stop-start;
}

void startCycles(CyclesData* time) 
{
    time->start = rdtsc();
}

float stopCycles(CyclesData* time) 
{
    time->stop = rdtsc();
    time->over = cycle_overhead;
    return (float)(time->stop-time->start-cycle_overhead)/(float)cpuclock;
}

void starttimer(TimerData* time)
{
    int i;
    i = gettimeofday(&(time->before),NULL);

#ifdef DEBUG
    printf("Seconds: %ld \t uSeconds: %ld \n", before.tv_sec, before.tv_usec);
#endif
}

void stoptimer(TimerData* time)
{
    int i;
    i = gettimeofday(&(time->after),NULL);

#ifdef DEBUG
    printf("Seconds: %ld \t uSeconds: %ld \n", after.tv_sec, after.tv_usec);
#endif
}

float printtimer(TimerData* time_data)
{
    long int sec;
    float time;

    sec = time_data->after.tv_sec - time_data->before.tv_sec;

    time = (((sec*1000000)+time_data->after.tv_usec)-time_data->before.tv_usec) * 0.001F;

#ifdef VERBOSE
    printf("*******************************************\n");
    printf("TIME [ms]\t:\t %f \n", time );
    printf("*******************************************\n\n");
#endif

    return time;
}


float cpuClockFrequency()
{
    static float frequency = -1.;
    int ret;

    if (frequency < 0) {
	FILE *output = popen("grep 'cpu MHz' /proc/cpuinfo | awk -F : '{print $2}'", "r");
	frequency = 0.0;
	ret = fscanf(output, "%f\n", &frequency);
	pclose(output);
	/* convert to Hz */
	frequency *= 1000000;
    }
    return frequency;
}


