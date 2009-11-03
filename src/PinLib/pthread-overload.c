#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sched.h>
#include <bits/pthreadtypes.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

#ifdef COLOR
#include <textcolor.h>
#endif

#define str(x) #x
#define MAX_CORES 32

extern int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const cpu_set_t *cpuset);

static char * sosearchpaths[] = {
#ifdef LIBPTHREADLOCATION
    str(LIBPTHREADLOCATION),
#endif
    "/lib64/tls/libpthread.so.0",/* sles9 x86_64 */
    "libpthread.so.0",           /* Ubuntu */
    NULL
};

int
pthread_create(pthread_t* thread,
        const pthread_attr_t* attr,
        void* (*start_routine)(void *),
        void * arg)
{
    void *handle;
    char *error;
    int (*rptc) (pthread_t *, const pthread_attr_t *, void* (*start_routine)(void *), void *);
    int ret;
    static int reallpthrindex = 0;
    static int npinned = 0;
    static int ncalled = 0;
    static int pin_ids[MAX_CORES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    static int skipMask = 0;


#ifdef COLOR
    color_on(BRIGHT, COLOR);
#endif
    /* On first entry: Get Evironment Variable and initialize pin_ids */
    if (ncalled == 0) 
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
            printf("[pthread wrapper] ERROR: Environment Variabel LIKWID_PIN not set!\n");
        }

        printf("[pthread wrapper] PIN_MASK: ");
        for (int i=0;i<ncpus;i++) 
        {
            printf("%d->%d  ",i,pin_ids[i]); 
        }
        printf("\n");

        str = getenv("LIKWID_SKIP");
        if (str != NULL) 
        {
            skipMask = strtoul(str, &str, 10);
        }
        else 
        {
            printf("[pthread wrapper] ERROR: Environment Variabel LIKWID_SKIP not set!\n");
        }
        printf("[pthread wrapper] SKIP MASK: 0x%X\n",skipMask);
    } 

    /* Handle dll related stuff */
    do 
    {
        handle = dlopen(sosearchpaths[reallpthrindex], RTLD_LAZY);
        if (handle) 
        {
            printf("[pthread wrapper %d] Notice: Using %s \n ",
                    ncalled, sosearchpaths[reallpthrindex]);
            break;
        }
        if (sosearchpaths[reallpthrindex] != NULL) 
        {
            reallpthrindex++;
        }
    }

    while (sosearchpaths[reallpthrindex] != NULL);

    if (!handle) 
    {
        printf("%s\n", dlerror());
        return -1;
    }

    dlerror();    /* Clear any existing error */
    rptc = dlsym(handle, "pthread_create");

    if ((error = dlerror()) != NULL)  
    {
        printf("%s\n", error);
        return -2;
    }

    ret = (*rptc)(thread, attr, start_routine, arg);

    /* After thread creation pin the thread */
    if (ret == 0) 
    {
        cpu_set_t cpuset;

        if (skipMask&(1<<(ncalled))) 
        {
            printf("\tthreadid %lu -> SKIP \n", *thread);
        }
        else 
        {
            CPU_ZERO(&cpuset);
            CPU_SET(pin_ids[npinned], &cpuset);
            printf("\tthreadid %lu -> core %d - ", *thread, pin_ids[npinned]);

            if (pthread_setaffinity_np(*thread, sizeof(cpu_set_t), &cpuset)) 
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
#ifdef COLOR
    color_reset();
#endif

    fflush(stdout);
    ncalled++;
    dlclose(handle);

    return ret;
}

