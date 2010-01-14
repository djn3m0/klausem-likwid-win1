/*
 * ===========================================================================
 *
 *       Filename:  applicationPin.c
 *
 *    Description:  An application to pin a program including threads
 *
 *        Version:  <VERSION>
 *        Created:  <DATE>
 *
 *         Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *        Company:  RRZE Erlangen
 *        Project:  likwid
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
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#include <types.h>

#ifdef COLOR
#include <textcolor.h>
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define HELP_MSG \
printf("likwid-pin --  Version %d.%d \n\n",VERSION,RELEASE); \
printf("\n"); \
printf("Supported Options:\n"); \
printf("-h\t Help message\n"); \
printf("-v\t Version information\n"); \
printf("-c\t comma separated processor ids\n"); \
printf("-s\t bitmask with threads to skip\n\n")

#define VERSION_MSG \
printf("likwid-pin   %d.%d \n\n",VERSION,RELEASE)
    

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

#ifdef COLOR
    color_on(BRIGHT, COLOR);
#endif
	CPU_ZERO(&cpuset);
	CPU_SET(cpuid, &cpuset);
	printf("[likwid-pin] Main PID -> core %d - ",  cpuid);
	if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) == -1) 
	{
		printf("sched_setaffinity failed : %s \n",strerror(errno));
	}
	else 
	{
		printf("OK\n");
	}
#ifdef COLOR
    color_reset();
#endif
}


int main (int argc, char** argv)
{ 
    int c;
	int skipMask = 0;
    char * typeString = NULL;
    char * pinString = NULL;
    char * skipString = NULL;
    int verbose = 0;
    int numThreads=0;
    /* FIXME It should be checked for size to prevent buffer overflow on threads */
    int threads[MAX_NUM_THREADS];
    int i;

    if (argc ==  1) { 
        HELP_MSG; 
        exit (EXIT_SUCCESS);    
    }

    skipString = (char*) malloc(10*sizeof(char));
    pinString = (char*) malloc(100*sizeof(char));

    while ((c = getopt (argc, argv, "+c:s:t:hvV")) != -1)
    {
        switch (c)
        {
            case 'h':
                HELP_MSG;
                exit (EXIT_SUCCESS);    
            case 'v':
                VERSION_MSG;
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
            case 's':
                skipMask = strtoul(optarg,NULL,16);
                break;
            case 'V':
                verbose = 1;
                break;
            default:
                HELP_MSG;
                exit(EXIT_FAILURE);
        }
    }

    if (typeString == NULL)
    {
        typeString = (char*) malloc(30*sizeof(char));
        strcpy(typeString,"NoType");
    }

	/* CPU List:
	 * pthread (default): pin main pid + all thread tids
	 *
	 * OpenMP: Pin OMP_NUM_THREADS
	 * intel openmp: pin main pid + all thread tids (skip thread 1)
	 * gcc openmp: pin main pid + all thread tids (one less)
	 */

    if (!strcmp("intel",typeString)) 
    {
		skipMask = 0x1;
    }

	if (numThreads > 1)
	{
		sprintf(pinString,"%d",threads[1]);

		for (i=2; i < numThreads;i++)
		{
			sprintf(pinString,"%s,%d",pinString,threads[i]);
		}

        sprintf(skipString,"%d",skipMask);
        setenv("LIKWID_PIN",pinString , 1);
        setenv("LIKWID_SKIP",skipString , 1);
        setenv("LD_PRELOAD",TOSTRING(LIBLIKWIDPIN), 1);

        if (verbose)
        {
            printf("Threads: %d \n",numThreads);
            printf("Pin list: %s\n",pinString);
            printf("Skip mask: %s\n",skipString);
        }
	}

	pinPid(threads[0]);
    fflush(stdout);

	argv +=  optind;
	execvp(argv[0], argv);
	perror("execvp");
	fprintf(stderr,"failed to execute %s\n", argv[0]);

    return EXIT_SUCCESS;
}

