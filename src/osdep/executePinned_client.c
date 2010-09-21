#include <osdep/executePinned.h>
#include <osdep/setenv.h>
#include <osdep/strtok_r.h>
#include <osdep/affinity.h>
#include <types.h>

#include <stdlib.h>
#include <stdio.h>

static int executePinned_ncalled = 0;

int executePinned_pinNextThread() {
	static int pin_ids[MAX_NUM_THREADS];
	static int skipMask = 0;
	static int npinned = 0;

	if (executePinned_ncalled == 0) 
    {
        char *str = getenv("LIKWID_PIN");
        char *token, *saveptr;
        char *delimiter = ",";
        int i = 0;
        int ncpus = 0;
        printf("[pthread wrapper] ");

        if (str != NULL) 
        {
            token = str;
            while (token) 
            {
                token = strtok_r(str,delimiter,&saveptr);
                str = NULL;
                if (token) 
                {
                    ncpus++;
                    pin_ids[i++] = strtoul(token, &token, 10);
                }
            }
        }
        else 
        {
            printf("[pthread wrapper] ERROR: Environment Variable LIKWID_PIN not set!\n");
        }

        printf("[pthread wrapper] PIN_MASK: ");
		{
			int i;
			for (i=0;i<ncpus;i++) 
			{
				printf("%d->%d  ",i,pin_ids[i]); 
			}
		}
        printf("\n");

        str = getenv("LIKWID_SKIP");
        if (str != NULL) 
        {
            skipMask = strtoul(str, &str, 10);
        }
        else 
        {
            printf("[pthread wrapper] ERROR: Environment Variable LIKWID_SKIP not set!\n");
        }
        printf("[pthread wrapper] SKIP MASK: 0x%X\n",skipMask);
    }

	 /* pin the thread */
    
    {
		int threadId = executePinned_ncalled+1;

        if (skipMask&(1<<(executePinned_ncalled))) 
        {
            printf("\tthreadid %lu -> SKIP \n", threadId);
        }
        else 
        {
            printf("\tthreadid %lu -> core %d - ", threadId, pin_ids[npinned]);

			if (affinity_pinThread(pin_ids[npinned]) == FALSE)
            {
                perror("pthread_setaffinity_np failed");
            }
            else 
            {
                printf("OK\n");
            }
            npinned++;
        }
    }

	executePinned_ncalled++;

	return 0;
}

int executePinned_currentThreadId() {
	return executePinned_ncalled;
}
