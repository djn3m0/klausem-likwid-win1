#include <stdlib.h>
#include <stdio.h>
#include <types.h>
#include <msr.h>
#include <textcolor.h>
#include <cpuFeatures.h>

#define MSR_IA32_MISC_ENABLE      0x1A0

void cpuFeatures_print(void)
{
    uint64 flags = readMSR(0,MSR_IA32_MISC_ENABLE);

    printf(HLINE);
    printf("Fast-Strings: \t\t\t");
    if (flags & 1) {
	color_on(BRIGHT,GREEN);printf("enabled\n");color_reset();
    } else {
	color_on(BRIGHT,RED);printf("disabled\n");color_reset();
    }
    printf("Automatic Thermal Control: \t");
    if (flags & (1ULL<<3)) {
	color_on(BRIGHT,GREEN);printf("enabled\n");color_reset();
    } else {
	color_on(BRIGHT,RED);printf("disabled\n");color_reset();
    }
    printf("Performance monitoring: \t");
    if (flags & (1ULL<<7)) {
	color_on(BRIGHT,GREEN);printf("enabled\n");color_reset();
    } else {
	color_on(BRIGHT,RED);printf("disabled\n");color_reset();
    }
    printf("Hardware Prefetcher: \t\t");
    if (flags & (1ULL<<9)) {
	color_on(BRIGHT,RED);printf("disabled\n");color_reset();
    } else {
	color_on(BRIGHT,GREEN);printf("enabled\n");color_reset();
    }
    printf("PEBS: \t\t\t\t");
    if (flags & (1ULL<<12)) {
	color_on(BRIGHT,RED);printf("not supported\n");color_reset();
    } else {
	color_on(BRIGHT,GREEN);printf("supported\n");color_reset();
    }
    printf("Intel Enhanced SpeedStep: \t");
    if (flags & (1ULL<<16)) {
	color_on(BRIGHT,GREEN);printf("enabled\n");color_reset();
    } else {
	color_on(BRIGHT,RED);printf("disabled\n");color_reset();
    }
    printf("Adjacent Cache Line Prefetch: \t");
    if (flags & (1ULL<<19)) {
	color_on(BRIGHT,RED);printf("disabled\n");color_reset();
    } else {
	color_on(BRIGHT,GREEN);printf("enabled\n");color_reset();
    }
    printf("DCU Prefetcher: \t\t");
    if (flags & (1ULL<<37)) {
	color_on(BRIGHT,RED);printf("disabled\n");color_reset();
    } else {
	color_on(BRIGHT,GREEN);printf("enabled\n");color_reset();
    }
    printf("Intel Dynamic Acceleration: \t");
    if (flags & (1ULL<<38)) {
	color_on(BRIGHT,RED);printf("disabled\n");color_reset();
    } else {
	color_on(BRIGHT,GREEN);printf("enabled\n");color_reset();
    }
    printf("IP Prefetcher: \t\t\t");
    if (flags & (1ULL<<39)) {
	color_on(BRIGHT,RED);printf("disabled\n");color_reset();
    } else {
	color_on(BRIGHT,GREEN);printf("enabled\n");color_reset();
    }
    printf(HLINE);
}

void cpuFeatures_enable(cpuFeatureType type)
{
    uint64 flags = readMSR(0,MSR_IA32_MISC_ENABLE);

    switch ( type ) {
	case HW_PREFETCHER:	
	    flags &= ~(1ULL<<9);
	    break;

	case CL_PREFETCHER:	
	    flags &= ~(1ULL<<19);
	    break;

	case DCU_PREFETCHER:	
	    flags &= ~(1ULL<<37);
	    break;

	case IP_PREFETCHER:	
	    flags &= ~(1ULL<<39);
	    break;

	default:	
	    break;
    }

}


void cpuFeatures_disable(cpuFeatureType type)
{
    uint64 flags = readMSR(0,MSR_IA32_MISC_ENABLE);

    switch ( type ) {
	case HW_PREFETCHER:	
	    flags |= (1ULL<<9);
	    break;

	case CL_PREFETCHER:	
	    flags |= (1ULL<<19);
	    break;

	case DCU_PREFETCHER:	
	    flags |= (1ULL<<37);
	    break;

	case IP_PREFETCHER:	
	    flags |= (1ULL<<39);
	    break;

	default:	
	    break;
    }

}

