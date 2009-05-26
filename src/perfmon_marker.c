#include <types.h>
#include <cpuid.h>
#include <msr.h>
#include <registers.h>

void perfmon_marker_start_counters(int cpu_id)
{
    uint64 flags = 0x0ULL;
    cpuid_init();

    switch ( cpuid_info.family ) {
	case P6_FAMILY:	

	    writeMSR(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);
	    writeMSR(cpu_id, MSR_PERF_FIXED_CTR0, 0x0ULL);
	    writeMSR(cpu_id, MSR_PERF_FIXED_CTR1, 0x0ULL);

	    switch ( cpuid_info.model ) {
		case PENTIUM_M:	
		    break;

		case CORE_DUO:	
		    break;

		case CORE2_65:	

		case CORE2_45:	

		    writeMSR(cpu_id, MSR_PMC0 , 0x0ULL);
		    writeMSR(cpu_id, MSR_PMC1 , 0x0ULL);
		    writeMSR(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x300000003ULL);
		    writeMSR(cpu_id, MSR_PERF_GLOBAL_OVF_CTRL, 0x300000003ULL);
		    break;

		case NEHALEM:	

		    writeMSR(cpu_id, MSR_PMC0 , 0x0ULL);
		    writeMSR(cpu_id, MSR_PMC1 , 0x0ULL);
		    writeMSR(cpu_id, MSR_PMC2 , 0x0ULL);
		    writeMSR(cpu_id, MSR_PMC3 , 0x0ULL);
		    writeMSR(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x30000000FULL);
		    writeMSR(cpu_id, MSR_PERF_GLOBAL_OVF_CTRL, 0x30000000FULL);
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

		case SHANGHAI:	

		    writeMSR(cpu_id, MSR_AMD_PMC0 , 0x0ULL);
		    writeMSR(cpu_id, MSR_AMD_PMC1 , 0x0ULL);
		    writeMSR(cpu_id, MSR_AMD_PMC2 , 0x0ULL);
		    writeMSR(cpu_id, MSR_AMD_PMC3 , 0x0ULL);

		    flags = readMSR(cpu_id, MSR_AMD_PERFEVTSEL0);
		    flags |= (1<<22);  /* enable flag */
		    writeMSR(cpu_id, MSR_AMD_PERFEVTSEL0, flags);
		    flags = readMSR(cpu_id, MSR_AMD_PERFEVTSEL1);
		    flags |= (1<<22);  /* enable flag */
		    writeMSR(cpu_id, MSR_AMD_PERFEVTSEL1, flags);
		    flags = readMSR(cpu_id, MSR_AMD_PERFEVTSEL2);
		    flags |= (1<<22);  /* enable flag */
		    writeMSR(cpu_id, MSR_AMD_PERFEVTSEL2, flags);
		    flags = readMSR(cpu_id, MSR_AMD_PERFEVTSEL3);
		    flags |= (1<<22);  /* enable flag */
		    writeMSR(cpu_id, MSR_AMD_PERFEVTSEL3, flags);
		    break;

		default:	
		    break;
	    }

	default:	
	    break;
    }
}


void perfmon_marker_stop_counters(int cpu_id)
{
    uint64 flags;
    int i;

    switch ( cpuid_info.family ) {
	case P6_FAMILY:	

	    writeMSR(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);

	case NETBURST_FAMILY:	
	    break;

	case K10_FAMILY:	

	    flags = readMSR(cpu_id,MSR_AMD_PERFEVTSEL0);
	    flags &= ~(1<<22);  /* clear enable flag */
	    writeMSR(cpu_id, MSR_AMD_PERFEVTSEL0 , flags);
	    flags = readMSR(cpu_id,MSR_AMD_PERFEVTSEL1);
	    flags &= ~(1<<22);  /* clear enable flag */
	    writeMSR(cpu_id, MSR_AMD_PERFEVTSEL1 , flags);
	    flags = readMSR(cpu_id,MSR_AMD_PERFEVTSEL2);
	    flags &= ~(1<<22);  /* clear enable flag */
	    writeMSR(cpu_id, MSR_AMD_PERFEVTSEL2 , flags);
	    flags = readMSR(cpu_id,MSR_AMD_PERFEVTSEL3);
	    flags &= ~(1<<22);  /* clear enable flag */
	    writeMSR(cpu_id, MSR_AMD_PERFEVTSEL3 , flags);

	    break;

	default:	
	    break;
    }

}


