/*
 * ===========================================================================
 *
 *       Filename:  applicationPin.c
 *
 *    Description:  An application to pin a program including gernated threads
 *
 *        Version:  0.1
 *        Created:  11/03/2009
 *
 *         Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *        Company:  RRZE Erlangen
 *        Project:  HpcUtil
 *      Copyright:  Copyright (c) 2009, Jan Treibig
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License, v2, as
 *      published by the Free Software Foundation
 *     
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *     
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * ===========================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

#include <types.h>

#define MAX_NUM_THREADS 100

#define HELP_MSG \
printf("likwid-pin --  Version 0.1\n"); \
printf("\n"); \
printf("Supported Options:\n"); \
printf("-h\t Help message\n"); \
printf("-v\t verbose output\n"); \
printf("-c\t comma separated core ids to measure\n\n")

/* the next two functions were inspired and adopted from 
 * the taskset application in linux-util package */
static const char *nexttoken(const char *str,  int sep)
{
    if (str)
    {
        str = strchr(str, sep);
    }

    if (str)
    {
        str++;
    }

    return str;
}

static int cstr_to_cpuset(int* threads,  const char* str)
{
    int numThreads=0;
    const char *p, *q;
    q = str;

    while (p = q, q = nexttoken(q, ','), p) 
    {
        unsigned int rangeBegin;
        unsigned int rangeEnd;
        const char *c1, *c2;

        if (sscanf(p, "%u", &rangeBegin) < 1)
        {
            return 0;
        }

        rangeEnd = rangeBegin;

        c1 = nexttoken(p, '-');
        c2 = nexttoken(p, ',');

        if (c1 != NULL && (c2 == NULL || c1 < c2)) 
        {
            if (sscanf(c1, "%u", &rangeEnd) < 1)
            {
                return 0;
            }
            else if(rangeEnd > 1000)
            {
                return 0;
            }
        }

        if (!(rangeBegin <= rangeEnd))
        {
            return 0;
        }

        while (rangeBegin <= rangeEnd) {
            numThreads++;
            threads[numThreads-1] = rangeBegin;
            rangeBegin++;
        }
    }

    return numThreads;

}

void
pinPid(int cpuid)
{
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(cpuid, &cpuset);
	printf("[likwid-pin  0] Main PID -> core %d - ",  cpuid);
	if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) == -1) 
	{
		printf("sched_setaffinity failed : %s \n",strerror(errno));
	}
	else 
	{
		printf("OK\n");
	}
}

int main (int argc, char** argv)
{ 
    int cpu_id = 0;
    int c;
	int skipMask = 0;
    char * typeString = NULL;
    char * pinString = NULL;
    int numThreads=0;
    /* It should be checked for size to prevent buffer overflow on threads */
    int threads[MAX_NUM_THREADS];
    int i,j;

    if (argc ==  1) { HELP_MSG; }

    while ((c = getopt (argc, argv, "c:t:hv")) != -1)
    {
        switch (c)
        {
            case 'h':
                HELP_MSG;
                exit (EXIT_SUCCESS);    
            case 'c':
                numThreads = cstr_to_cpuset(threads, optarg);

                if(!numThreads)
                {
                    fprintf (stderr, "ERROR: Failed to parse cpu list.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 't':
                typeString = (char*) malloc((strlen(optarg)+10)*sizeof(char));
                strcpy(typeString,optarg);
                break;
            case 'v':
                perfmon_verbose = 1;
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
                HELP_MSG;
        }
    }

    for (i = 0; i< numThreads;i++)
    {
        for (j = 0; j< numThreads;j++)
        {
            if(i != j && threads[i] == threads[j])
            {
                fprintf (stderr, "ERROR: Cpu list is not unique.\n");
                exit(EXIT_FAILURE);
            }
        }
    }

	/* CPU List:
	 * pthread 0: pin main pid + all thread tids
	 * pthread 1: pin only all thread tids
	 *
	 * OpenMP: Pin OMP_NUM_THREADS
	 * intel openmp: pin main pid + all thread tids (skip thread 1)
	 * gcc openmp: pin main pid + all thread tids (one less)
	 */

    if (!strcmp("pthread",typeString)) 
    {
		skipMask = 0x0;
    }
    else if (!strcmp("omp_intel",typeString)) 
    {
		skipMask = 0x2;
    }
    else if (!strcmp("omp_gcc",typeString)) 
    {
		skipMask = 0x0;
    }

	if (numThreads > 1)
	{
		sprintf(pinString,"%d",threads[1]);

		for (i=2; i < numThreads;i++)
		{
			sprintf(pinString,"%s,%d",pinString,threads[i]);
		}
	}

	sprintf(skipString,"%d",skipMask);
	pinPid(threads[0]);
	setenv("PIN_CPUS",pinString , 1);
	setenv("SKIP_MASK",skipString , 1);

	argv += optind + 1;
	execvp(argv[0], argv);
	perror("execvp");
	fprintf(stderr,"failed to execute %s\n", argv[0]);

    return EXIT_SUCCESS;
}

