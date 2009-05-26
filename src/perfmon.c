#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <types.h>
#include <timer.h>
#include <msr.h>
#include <cpuid.h>
#include <perfmon.h>
#include <registers.h>

#include <perfmon_core2_events.h>
#include <perfmon_nehalem_events.h>
#include <perfmon_k10_events.h>

static int perfmon_setup_counter_thread(int,  perfmon_counter_index_t , char* );
#include <perfmon_core2.h>
#include <perfmon_nehalem.h>
#include <perfmon_k10.h>

/* global variables */
int num_arch_events;
int perfmon_verbose=0;

/* module variables */
static perfmon_group_t group_set = NOGROUP;
static const perfmon_hash_entry_t * event_hash;
static perfmon_thread_t summary;
static perfmon_thread_t* thread_data;
static CyclesData timeData;
static int num_threads;

/* external global variables */
extern int perfmon_verbose; /* defined in main.c */

/*
 * STATIC FUNCTIONS
 */

static int get_event(char* event_str,uint32* event,uint32* umask)
{
    int i=0;
    int found = 0;

    for (i=0; i< num_arch_events;i++){
	if (strcmp(event_str, event_hash[i].key) == 0){
	    *event = event_hash[i].event.event_id;
	    *umask = event_hash[i].event.umask;
	    found = 1;
	    break;
	}
    }

    if (found) {
	if (perfmon_verbose) printf ("Found event %s : Event_id 0x%02X Umask 0x%02X \n",
		event_str, *event, *umask);
	return TRUE;
    }else {
	return FALSE;
    }

}

static void perfmon_init_thread(int thread_id, int cpu_id)
{
    int i;

    for (i=0;i<NUM_PMC;i++) {
	thread_data[thread_id].counters[i].label = (char*) malloc(500*sizeof(char));
	thread_data[thread_id].counters[i].init = FALSE;
    }

    thread_data[thread_id].cpu_id = cpu_id;

    switch ( cpuid_info.family ) {
	case P6_FAMILY:	

	    switch ( cpuid_info.model ) {
		case CORE2_65:	

		case CORE2_45:	

		    perfmon_init_core2(&thread_data[thread_id]);
		    break;

		case NEHALEM:	

		    perfmon_init_nehalem(&thread_data[thread_id]);
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
		    perfmon_init_k10(&thread_data[thread_id]);
		    break;

		default:	
		    break;
	    }

	    break;

	default:	
	    break;
    }
}

static int perfmon_setup_counter_thread(int thread_id, perfmon_counter_index_t index, char* event_str)
{
    uint64 flags;
    uint32 umask, event;
    uint64 reg = thread_data[thread_id].counters[index].config_reg;
    int cpu_id = thread_data[thread_id].cpu_id;

    if (!get_event(event_str, &event, &umask)){
	printf("ERROR: Event %s not found for current architecture\n",event_str);
	return FALSE;
    }

    strcpy(thread_data[thread_id].counters[index].label, event_str);
    thread_data[thread_id].counters[index].init = TRUE;

    flags = readMSR(cpu_id,reg);
    flags &= ~(0xFFFFU); 

    if (cpuid_info.family == P6_FAMILY){
	writeMSR(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);
	/* Intel with standard 8 bit event mask: [7:0] */
	flags |= (umask<<8) + event;
    } else {
	/* AMD uses a 12 bit Event mask: [35:32][7:0] */
	flags |= ((uint64)(event>>8)<<32) + (umask<<8) + (event & ~(0xF00U));
    }

    if (perfmon_verbose) printf("[%d] perfmon_setup_counter: Write Register 0x%llX , Flags: 0x%llX \n",cpu_id,reg,flags);
    writeMSR(cpu_id, reg , flags);

    return TRUE;
}


static void perfmon_setup_group_thread(int thread_id, perfmon_group_t group)
{
    group_set = group;

    switch ( cpuid_info.family ) {
	case P6_FAMILY:	

	    switch ( cpuid_info.model ) {
		case PENTIUM_M:	
		    break;

		case CORE_DUO:	
		    break;

		case CORE2_65:	

		case CORE2_45:	

		    perfmon_setup_group_core2(thread_id,group_set);
		    break;

		case NEHALEM:	

		    perfmon_setup_group_nehalem(thread_id,group_set);
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
		    perfmon_setup_group_k10(thread_id,group_set);
		    break;

		default:	
		    break;
	    }

	default:	
	    break;
    }
}


static void perfmon_start_counters_thread(int thread_id)
{
    int i;
    uint64 flags, uflags;
    int cpu_id = thread_data[thread_id].cpu_id;

    switch ( cpuid_info.family ) {
	case P6_FAMILY:	

	    writeMSR(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);
	    writeMSR(cpu_id, MSR_PERF_FIXED_CTR0, 0x0ULL);
	    writeMSR(cpu_id, MSR_PERF_FIXED_CTR1, 0x0ULL);

	    /* Enable fixed counters */
	    flags  = 0x300000000ULL;
	    uflags = 0x1000000FFULL;

	    for (i=0;i<NUM_PMC;i++) {
		if (thread_data[thread_id].counters[i].init == TRUE) {
		    writeMSR(cpu_id, thread_data[thread_id].counters[i].counter_reg , 0x0ULL);
		    if (i < PMCU0) {
			flags |= (1<<i);  /* enable counter */
		    } else {
			uflags |= (1<<(i-PMCU0));  /* enable uncore counter */
		    }
		}
	    }
	    if (perfmon_verbose) printf("perfmon_start_counters: Write Register 0x%X , Flags: 0x%llX \n",MSR_PERF_GLOBAL_CTRL,flags);

	    switch ( cpuid_info.model ) {
		case PENTIUM_M:	
		    break;

		case CORE_DUO:	
		    break;

		case CORE2_65:	

		case CORE2_45:	

		    writeMSR(cpu_id, MSR_PERF_GLOBAL_CTRL, flags);
		    writeMSR(cpu_id, MSR_PERF_GLOBAL_OVF_CTRL, 0x300000003ULL);
		    break;

		case NEHALEM:	

		    if (perfmon_verbose) printf("perfmon_start_counters: Write Register 0x%X , Flags: 0x%llX \n",MSR_UNCORE_PERF_GLOBAL_CTRL,uflags);
		    writeMSR(cpu_id, MSR_PERF_GLOBAL_CTRL, flags);
		    writeMSR(cpu_id, MSR_UNCORE_PERF_GLOBAL_CTRL, uflags);
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

		    for (i=0;i<NUM_PMC;i++) {
			if (thread_data[thread_id].counters[i].init == TRUE) {
			    writeMSR(cpu_id, thread_data[thread_id].counters[i].counter_reg , 0x0ULL);
			    flags = readMSR(cpu_id,thread_data[thread_id].counters[i].config_reg);
			    flags |= (1<<22);  /* enable flag */
			    if (perfmon_verbose) printf("perfmon_start_counters: Write Register 0x%llX , Flags: 0x%llX \n",thread_data[thread_id].counters[i].config_reg,flags);
			    writeMSR(cpu_id, thread_data[thread_id].counters[i].config_reg , flags);
			}
		    }
		    break;

		default:	
		    break;
	    }

	default:	
	    break;
    }
}


static void perfmon_stop_counters_thread(int thread_id)
{
    uint64 flags;
    uint64 uncore_cycles;
    int i;
    int cpu_id = thread_data[thread_id].cpu_id;

    switch ( cpuid_info.family ) {
	case P6_FAMILY:	

	    writeMSR(cpu_id, MSR_PERF_GLOBAL_CTRL, 0x0ULL);
	    if (cpuid_info.model == NEHALEM) writeMSR(cpu_id, MSR_UNCORE_PERF_GLOBAL_CTRL, 0x0ULL);

	    for (i=0;i<NUM_PMC;i++) {
		if (thread_data[thread_id].counters[i].init == TRUE) {
		    thread_data[thread_id].pc[i] = readMSR(cpu_id, thread_data[thread_id].counters[i].counter_reg);
		}
	    }

	    thread_data[thread_id].cycles = readMSR(cpu_id, MSR_PERF_FIXED_CTR1);

	    flags = readMSR(cpu_id,MSR_PERF_GLOBAL_STATUS);
	    printf ("Status: 0x%llX \n",flags);
	    if((flags & 0x3) || (flags & (0x3ULL<<32)) ) {
		printf ("Overflow occured \n");
	    }

	case NETBURST_FAMILY:	
	    break;

	case K10_FAMILY:	

	    for (i=0;i<NUM_PMC;i++) {
		if (thread_data[thread_id].counters[i].init == TRUE) {
		    flags = readMSR(cpu_id,thread_data[thread_id].counters[i].config_reg);
		    flags &= ~(1<<22);  /* clear enable flag */
		    writeMSR(cpu_id, thread_data[thread_id].counters[i].config_reg , flags);
		    if (perfmon_verbose) printf("perfmon_stop_counters: Write Register 0x%llX , Flags: 0x%llX \n",thread_data[thread_id].counters[i].config_reg,flags);
		    thread_data[thread_id].pc[i] = readMSR(cpu_id, thread_data[thread_id].counters[i].counter_reg);
		}
	    }
	    thread_data[thread_id].cycles = thread_data[thread_id].pc[2];

	    break;

	default:	
	    break;
    }

}

/* 
 * EXTERNAL API
 */

void perfmon_print_results()
{
    int thread_id;
    float time;
    int i;

    for (i=0;i<NUM_PMC;i++) {
	summary.counters[i].label = (char*) malloc(500*sizeof(char));
	summary.pc[i] = 0;
	summary.counters[i].init = FALSE;
    }
    summary.cpu_id = 100;


    for (thread_id=0;thread_id<num_threads;thread_id++) {

	printf(HLINE);
	printf ("[%d] Cycles unhalted core: %llu \n",thread_data[thread_id].cpu_id,thread_data[thread_id].cycles);

	for (i=0;i<NUM_PMC;i++) {
	    if (thread_data[thread_id].counters[i].init == TRUE) {
		summary.pc[i] += thread_data[thread_id].pc[i];
		printf ("[%d] %s: %llu \n",thread_data[thread_id].cpu_id,thread_data[thread_id].counters[i].label,thread_data[thread_id].pc[i]);
	    }
	}
	printf(HLINE);
	printf("[%d] ==== Derived Metrics ====\n",thread_data[thread_id].cpu_id);
	printf(HLINE);
	time = (float)thread_data[thread_id].cycles/(float)cpuid_info.clock;
	printf ("[%d] Execution time: %e sec \n",thread_data[thread_id].cpu_id,time);

	switch ( cpuid_info.family ) {
	    case P6_FAMILY:	

		switch ( cpuid_info.model ) {
		    case PENTIUM_M:	
			break;

		    case CORE_DUO:	
			break;

		    case CORE2_65:	

		    case CORE2_45:	

			perfmon_print_results_core2(&thread_data[thread_id], group_set, time);
			break;

		    case NEHALEM:	

			perfmon_print_results_nehalem(&thread_data[thread_id], group_set, time);
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
			perfmon_print_results_k10(&thread_data[thread_id], group_set, time);
			break;

		    default:	
			break;
		}

	    default:	
		break;
	}

	printf(HLINE);
    }

    if ( num_threads > 1) {
	printf(HLINE);
	printf("==== SUMMARY RESULTS ====\n");
	printf(HLINE);
	printf ("[%d] RDTSC Cycles: %llu \n",summary.cpu_id,summary.cycles);
	for (i=0;i<NUM_PMC;i++) {
	    if (thread_data[0].counters[i].init == TRUE) {
		printf ("[%d] %s: %llu \n",summary.cpu_id,thread_data[0].counters[i].label,summary.pc[i]);
	    }
	}

	printf(HLINE);
	printf("==== Derived Metrics ====\n");
	printf(HLINE);
	time = (float)summary.cycles/(float)cpuid_info.clock;
	printf ("[%d] Execution time: %e sec \n",summary.cpu_id,time);
	switch ( cpuid_info.family ) {
	    case P6_FAMILY:	

		switch ( cpuid_info.model ) {
		    case PENTIUM_M:	
			break;

		    case CORE_DUO:	
			break;

		    case CORE2_65:	

		    case CORE2_45:	

			perfmon_print_results_core2(&summary, group_set, time);
			break;

		    case NEHALEM:	

			perfmon_print_results_nehalem(&summary, group_set, time);
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
			perfmon_print_results_k10(&summary, group_set, time);
			break;

		    default:	
			break;
		}

	    default:	
		break;
	}
	printf(HLINE);
    }
}

void perfmon_set_cycles(uint64 cycles)
{
    summary.cycles = cycles;

}

void perfmon_setup_group(perfmon_group_t group)
{
    int i;

    for (i=0;i<num_threads;i++) perfmon_setup_group_thread(i,group);
}

int perfmon_setup_counter( perfmon_counter_index_t index, char* event_str)
{
    int i;

    for (i=0;i<num_threads;i++){
	if (!perfmon_setup_counter_thread(i,index,event_str)) {
	    fprintf (stderr, "Unknown performance event %s\n",event_str);
	    return EXIT_FAILURE;
	}
    }

    return EXIT_SUCCESS;
}

void perfmon_start_all_counters()
{
    int i;

    for (i=0;i<num_threads;i++) perfmon_start_counters_thread(i);
    startCycles(&timeData);
}

void perfmon_stop_all_counters()
{
    int i;

    stopCycles(&timeData);
    for (i=0;i<num_threads;i++) perfmon_stop_counters_thread(i);

    perfmon_set_cycles(timeData.stop-timeData.start-timeData.over);
}



void perfmon_start_counters(int thread_id)
{
    perfmon_start_counters_thread(thread_id);

}


void perfmon_stop_counters(int thread_id)
{
    perfmon_stop_counters_thread(thread_id);

}



void perfmon_init(int num_threads_local, int threads[])
{
    int i;

    num_threads = num_threads_local;
    thread_data = (perfmon_thread_t*) malloc(num_threads * sizeof(perfmon_thread_t));
    initTimer();

    switch ( cpuid_info.family ) {
	case P6_FAMILY:	

	    switch ( cpuid_info.model ) {
		case PENTIUM_M:	
		    fprintf(stderr, "Unsupported Processor!\n");
		    exit(EXIT_FAILURE);
		    break;

		case CORE_DUO:	
		    fprintf(stderr, "Unsupported Processor!\n");
		    exit(EXIT_FAILURE);
		    break;

		case CORE2_65:	

		case CORE2_45:	

		    event_hash = core2_arch_events;
		    num_arch_events = NUM_ARCH_EVENTS_CORE2;
		    break;

		case NEHALEM:	

		    event_hash = nehalem_arch_events;
		    num_arch_events = NUM_ARCH_EVENTS_NEHALEM;
		    break;

		default:	
		    fprintf(stderr, "Unsupported Processor!\n");
		    exit(EXIT_FAILURE);
		    break;
	    }
	    break;

	case NETBURST_FAMILY:	
	    fprintf(stderr, "Unsupported Processor!\n");
	    exit(EXIT_FAILURE);
	    break;

	case K10_FAMILY:	
	    switch ( cpuid_info.model ) {
		case BARCELONA:	

		case SHANGHAI:	
		    event_hash = k10_arch_events;
		    num_arch_events = NUM_ARCH_EVENTS_K10;
		    break;

		default:	
		    fprintf(stderr, "Unsupported Processor!\n");
		    exit(EXIT_FAILURE);
		    break;
	    }

	    break;

	default:	
	    fprintf(stderr, "Unsupported Processor!\n");
	    exit(EXIT_FAILURE);
	    break;
    }


    for (i=0;i<num_threads;i++) perfmon_init_thread(i,threads[i]);
}



