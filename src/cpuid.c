#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#include <cpuid.h>

static char* pentium_m_str = "Intel Pentium M processor";
static char* core_duo_str = "Intel Core Duo processor";
static char* core_2a_str = "Intel Core 2 65nm processor";
static char* core_2b_str = "Intel Core 2 45nm processor";
static char* nehalem_str = "Intel Core i7 processor";
static char* barcelona_str = "AMD Barcelona processor";
static char* shanghai_str = "AMD Shanghai processor";

cpu_info cpuid_info;


extern uint64 rdtsc(void);


static uint64 get_cpu_speed(void)
{
   uint64 tsc1, tsc2;
   struct timeval tv1, tv2;
   struct timezone tzp;
   struct timespec delay = { 0, 200000000 };  //  calibration time: 200 ms
    
   tsc1 = rdtsc();
   gettimeofday( &tv1, &tzp);
   nanosleep( &delay, NULL);
   tsc2 = rdtsc();
   gettimeofday( &tv2, &tzp);
  
   return (tsc2 - tsc1) * 1000000 / 
          (((uint64)tv2.tv_sec * 1000000 + tv2.tv_usec) -
           ((uint64)tv1.tv_sec * 1000000 + tv1.tv_usec)  );
}

void cpuid_init (void)
{
    uint32 eax, ebx, ecx, edx;
    cpuid_info.features = (char*) malloc(20*sizeof(char));

    asm( "movl $0x01, %%eax\n\t "
	    "cpuid\n\t"
	    "movl %%eax, %0\n\t"
	    "movl %%ecx, %1\n\t" 
	    "movl %%edx, %2\n\t" 
	    : "=r" (eax), "=r" (ecx), "=r" (edx)
	    :
	    : "%eax","%ecx","%edx");

    cpuid_info.family = ((eax>>8)&0xFU) + ((eax>>20)&0xFFU);
    cpuid_info.model = (((eax>>16)&0xFU)<<4) + ((eax>>4)&0xFU);
    cpuid_info.stepping =  (eax&0xFU);
    cpuid_info.clock =  get_cpu_speed();

    switch ( cpuid_info.family ) {
	case P6_FAMILY:	

	    switch ( cpuid_info.model ) {
		case PENTIUM_M:	
		    cpuid_info.name = pentium_m_str;
		    break;

		case CORE_DUO:	
		    cpuid_info.name = core_duo_str;
		    break;

		case CORE2_65:	
		    cpuid_info.name = core_2a_str;
		    break;

		case CORE2_45:	
		    cpuid_info.name = core_2b_str;
		    break;

		case NEHALEM:	
		    cpuid_info.name = nehalem_str;
		    break;

		default:	
		    break;
	    }
	    break;

	case NETBURST_FAMILY:	
	    break;

	case K10_FAMILY:	
	    switch ( cpuid_info.model ) {
		case BARCELONA:	
		    cpuid_info.name = barcelona_str;
		    break;

		case SHANGHAI:	
		    cpuid_info.name = shanghai_str;
		    break;

		default:	
		    break;
	    }

	    break;

	default:	
	    break;
    }

    /* check for features */

    if (edx & (1<<25)) strcpy(cpuid_info.features, "SSE ");
    if (edx & (1<<26)) strcat(cpuid_info.features, "SSE2 ");
    if (ecx & (1<<0))  strcat(cpuid_info.features, "SSE3 ");
    if (ecx & (1<<19)) strcat(cpuid_info.features, "SSE4.1 ");
    if (ecx & (1<<20)) strcat(cpuid_info.features, "SSE4.2 ");

    if( cpuid_info.family == P6_FAMILY) {
	/* check performance monitor features */

	asm( "movl $0x0A, %%eax\n\t "
		"cpuid\n\t"
		"movl %%eax, %0\n\t"
		"movl %%ebx, %1\n\t" 
		"movl %%edx, %2\n\t" 
		: "=r" (eax), "=r" (ebx), "=r" (edx)
		:
		: "%eax","%ebx","%edx");


	cpuid_info.perf_version   =  (eax&0xFFU);
	cpuid_info.perf_num_ctr   =   ((eax>>8)&0xFFU);
	cpuid_info.perf_width_ctr =  ((eax>>16)&0xFFU);
	cpuid_info.perf_num_fixed_ctr =  (edx&0xFU);
    }

}


