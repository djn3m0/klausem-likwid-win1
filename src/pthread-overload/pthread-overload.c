/*
 * ===========================================================================
 *
 *      Filename:  pthread_overload.c
 *
 *      Description:  Overloaded library for pthread_create call. 
 *                    Implements pinning of threads together with likwid-pin.
 *
 *      Version:  <VERSION>
 *      Created:  <DATE>
 *
 *      Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *      Company:  RRZE Erlangen
 *      Project:  likwid
 *      Copyright:  Copyright (c) 2010, Jan Treibig
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
#include <osdep/executePinned.h>

#ifdef COLOR
#include <textcolor.h>
#endif

#define str(x) #x

extern int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const cpu_set_t *cpuset);

static char * sosearchpaths[] = {
#ifdef LIBPTHREAD
    str(LIBPTHREAD),
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

#ifdef COLOR
    color_on(BRIGHT, COLOR);
#endif

    /* Handle dll related stuff */
    do 
    {
        handle = dlopen(sosearchpaths[reallpthrindex], RTLD_LAZY);
        if (handle) 
        {
            printf("[pthread wrapper %d] Notice: Using %s \n ",
                    executePinned_currentThreadId()+1, sosearchpaths[reallpthrindex]);
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

	/* On first entry: Get Evironment Variable and initialize pin_ids */
	if (ret == 0) {
		executePinned_pinNextThread();
	}
#ifdef COLOR
    color_reset();
#endif

    fflush(stdout);
    dlclose(handle);

    return ret;
}

