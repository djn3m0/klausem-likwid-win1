#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

#include <types.h>
#include <cpuid.h>


#define HELP_MSG \
    printf("\ncpuTopology --  Version 0.1\n\n"); \
printf("A tool to print the thread and cache toppology on Intel CPUS.\n"); \
printf("Options:\n"); \
printf("-h\t Help message\n"); \
printf("-c\t list cache information\n"); \
printf("-g\t graphical output\n"); \
exit(0);

int main (int argc, char** argv)
{ 
    int optGraphical = 0;
    int optCaches = 0;
    int c;
    int i;

    while ((c = getopt (argc, argv, "hcg")) != -1)
    {
        switch (c)
        {
            case 'h':
                HELP_MSG
                    exit (EXIT_SUCCESS);    
            case 'g':
                optGraphical = 1;
                break;
            case 'c':
                optCaches = 1;
                break;
            case '?':
                if (isprint (optopt))
                {
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                }
                else
                {
                    fprintf (stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
                }
                return EXIT_FAILURE;
            default:
                HELP_MSG
        }
    }

    cpuid_init();
    printf(HLINE);
    printf("CPU name:\t%s \n",cpuid_info.name);
    printf("CPU clock:\t%llu Hz \n\n", cpuid_info.clock);

    cpuid_initTopology();
    cpuid_initCacheTopology();

    printf("Total hardware threads:\t%u \n", cpuid_topology.numHWThreads);
    printf("Sockets:\t\t%u \n", cpuid_topology.numSockets);
    printf("Cores per socket:\t%u \n", cpuid_topology.numCoresPerSocket);
    printf("Threads per core:\t%u \n", cpuid_topology.numThreadsPerCore);

    printf(HLINE);
    printf("HWThread\tThread\t\tCore\t\tSocket\n");
    for (i=0; i< cpuid_topology.numHWThreads; i++)
    {
        printf("%d\t\t%d\t\t%d\t\t%d\n",i
                ,cpuid_topology.threadPool[i].threadId
                ,cpuid_topology.threadPool[i].coreId
                ,cpuid_topology.threadPool[i].packageId);
    }
    printf(HLINE);
    for (i=0; i< cpuid_topology.numCacheLevels; i++)
    {
        printf("Level:\t %d\n",cpuid_topology.cacheLevels[i].level);
        if (cpuid_topology.cacheLevels[i].size < 1048576)
        {
            printf("Size:\t %d kB\n",cpuid_topology.cacheLevels[i].size/1024);
        }
        else 
        {
            printf("Size:\t %d MB\n",cpuid_topology.cacheLevels[i].size/1048576);
        }

        switch (cpuid_topology.cacheLevels[i].type) {
            case DATACACHE:	
                printf("Type:\t Data cache\n");
                break;

            case INSTRUCTIONCACHE:	
                printf("Type:\t Instruction cache\n");
                break;

            case UNIFIEDCACHE:	
                printf("Type:\t Unified cache\n");
                break;
        }
        printf("Associativity:\t %d\n",cpuid_topology.cacheLevels[i].associativity);
        printf("Number of sets:\t %d\n",cpuid_topology.cacheLevels[i].sets);
        printf("Cache line size: %d\n",cpuid_topology.cacheLevels[i].lineSize);
        if(cpuid_topology.cacheLevels[i].inclusive)
        {
            printf("Non Inclusive cache\n");
        }
        else
        {
            printf("Inclusive cache\n");
        }
        printf("Shared among %d threads\n",cpuid_topology.cacheLevels[i].threads);


        printf(HLINE);
    }

    printf(HLINE);

    return EXIT_SUCCESS;
}
