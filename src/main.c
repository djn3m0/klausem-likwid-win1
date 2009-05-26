#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

#include <types.h>
#include <cpuid.h>
#include <cpuFeatures.h>
#include <perfmon.h>

#define MAX_NUM_THREADS 100

#define HELP_MSG \
printf("hpcUtil --  Version 0.3\n"); \
printf("\n"); \
printf("Supported Options:\n"); \
printf("-h\t Help message\n"); \
printf("-v\t verbose output\n"); \
printf("-i\t print cpu info\n"); \
printf("-p\t pin to specific core \n"); \
printf("-m\t use markers inside code \n"); \
printf("-g\t performance group [STD FLOPS_SP L1 L2 L3 MEM DATA BRANCH TLB CPI] or event tag\n"); \
printf("-t\t comma separated core ids to measure\n\n"); \
exit(0);

extern int perfmon_verbose;

int main (int argc, char** argv)
{ 
    int opt_info = 0;
    int opt_threaded = 0;
    int opt_uncore_event = 0;
    int opt_use_marker = 0;
    int cpu_id = 0;
    int c;
    char * cmd_str;
    char * custom_event;
    cpu_set_t set;
    perfmon_group_t group= STD;
    char *token, *saveptr, *str;
    char *delimiter = ",";
    int num_threads=0;
    int threads[MAX_NUM_THREADS];

    if (argc ==  1) { HELP_MSG }

    while ((c = getopt (argc, argv, "t:g:u:p:hmvi")) != -1) {
	switch (c)
	{
	    case 'h':
		HELP_MSG
		    exit (EXIT_SUCCESS);    
	    case 'g':
		if (!strcmp("STD",optarg)) {
		    group = STD;
		} else if (!strcmp("FLOPS_SP",optarg)) {
		    group = FLOPS_SP;
		} else if (!strcmp("L1",optarg)) {
		    group = L1;
		} else if (!strcmp("L2",optarg)) {
		    group = L2;
		} else if (!strcmp("L3",optarg)) {
		    group = L3;
		} else if (!strcmp("MEM",optarg)) {
		    group = MEM;
		} else if (!strcmp("DATA",optarg)) {
		    group = DATA;
		} else if (!strcmp("BRANCH",optarg)) {
		    group = BRANCH;
		} else if (!strcmp("TLB",optarg)) {
		    group = TLB;
		} else if (!strcmp("CPI",optarg)) {
		    group = CPI;
		} else  {
		    custom_event = (char*) malloc((strlen(optarg)+10)*sizeof(char));
		    strcpy(custom_event,optarg);
		    group = NOGROUP;
		}
		break;
	    case 't':
		opt_threaded = 1;
		str = optarg;
		token = str;
		while (token) {
		    token = strtok_r(str,delimiter,&saveptr);
		    str = NULL;
		    if (token) {
			num_threads++;
			threads[num_threads-1] = atoi(token);
		    }
		}

		break;
	    case 'u':
		opt_uncore_event = 1;
		custom_event = optarg;
		group = NOGROUP;
		break;
	    case 'v':
		perfmon_verbose = 1;
		break;
	    case 'm':
		opt_use_marker = 1;
		break;
	    case 'i':
		opt_info = 1;
		perfmon_verbose = 1;
		break;
	    case 'p':
		cpu_id = atoi(optarg);
		break;
	    case '?':
		if (isprint (optopt))
		    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
		else
		    fprintf (stderr,
			    "Unknown option character `\\x%x'.\n",
			    optopt);
		return EXIT_FAILURE;
	    default:
		HELP_MSG
	}
    }

    cpuid_init();
    printf(HLINE);
    printf("CPU name:\t%s \n",cpuid_info.name);
    printf("CPU clock:\t%llu Hz \n", cpuid_info.clock);
    if (perfmon_verbose) {
	printf("CPU family:\t%u \n",cpuid_info.family);
	printf("CPU model:\t%u \n", cpuid_info.model);
	printf("CPU stepping:\t%u \n", cpuid_info.stepping);
	printf("CPU features:\t%s \n", cpuid_info.features);

	if( cpuid_info.family == P6_FAMILY) {
	    printf(HLINE);
	    printf("PERFMON version:\t%u \n",cpuid_info.perf_version);
	    printf("PERFMON number of counters:\t%u \n",cpuid_info.perf_num_ctr);
	    printf("PERFMON width of counters:\t%u \n",cpuid_info.perf_width_ctr);
	    printf("PERFMON number of fixed counters:\t%u \n",cpuid_info.perf_num_fixed_ctr);
	    cpuFeatures_print();
	}
    }
    printf(HLINE);

    if (opt_info) return EXIT_SUCCESS;

    if (optind == argc) {
	printf("WARNING: You have to specify a program to measure as argument!\n");
	return EXIT_SUCCESS;
    }

    cmd_str = (char*) malloc((strlen(argv[optind])+200)*sizeof(char));

    if(!opt_threaded) {
	sprintf(cmd_str,"taskset 0x%08lx %s",(1UL<<cpu_id),argv[optind]);
	CPU_ZERO(&set);
	CPU_SET(cpu_id,&set);
	sched_setaffinity(0, sizeof(cpu_set_t), &set);
	num_threads = 1;
	threads[0]=0;
    } else {
	sprintf(cmd_str,"%s",argv[optind]);
    }

    if (perfmon_verbose) printf("Executing: %s \n",cmd_str);

    perfmon_init(num_threads, threads);

    if (group != NOGROUP) {
	perfmon_setup_group(group);
    } else {
	if (opt_uncore_event) {
	    perfmon_setup_counter(PMCU0,custom_event);
	} else {
	    perfmon_setup_counter(PMC0,custom_event);
	}
    }

    if (!opt_use_marker) perfmon_start_all_counters();
    system(cmd_str);
    perfmon_stop_all_counters();
    perfmon_print_results();

    return EXIT_SUCCESS;
}

