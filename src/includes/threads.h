/**
 *      @file  threads.h
 *      @brief  Wrapper module to the pthread library
 *
 *   A module to ease the usage of the pthread library.
 *   The following terms are used:
 *   - Thread: A lightweight process belonging to a parent process and sharing
 *             its address space with other threads
 *   - Thread Group: A subset of all threads. In the current implementation the 
 *                   total number of threads must be a multiple of thread
 *                   groups. At the moment there is the same number of threads in
 *                   all groups.
 *
 *   Each thread gets is data passed in a struct of Type ThreadData. Application
 *   specific data must be provided in a struct ThreadUserData. The thread data
 *   can be accessed globally in the array threads_data.
 *   Usage without thread groups:
 *
 *   @code
 *   threads_init(4);
 *   threads_registerDataAll(&myData, NULL);
 *   threads_create(thread_callback);
 *   threads_destroy();
 *   @endcode
 *
 *     @author  Jan Treibig (jt), jan.treibig@gmail.com
 *
 *   @internal
 *     Created  08/07/2009
 *    Revision  ---
 *    Compiler  gcc
 *     Company  RRZE Erlangen
 *   Copyright  Copyright (c) 2009, Jan Treibig
 *
 * This source code is released for free distribution under the terms of the
 * GNU General Public License as published by the Free Software Foundation.
 * ============================================================================
 */
#ifndef THREADS_H
#define THREADS_H

#include <types.h>
#include <pthread.h>
#include <threads_types.h>

#define THREADS_BARRIER pthread_barrier_wait(&threads_barrier)

extern pthread_barrier_t threads_barrier;
extern ThreadData* threads_data;
extern ThreadGroup* threads_groups;


/**
 * @brief  Initialization of the thread module
 * @param  numberOfThreads  The total number of threads
 */
extern void threads_init(int numberOfThreads);

/**
 * @brief  Create all threads
 * @param  startRoutine thread entry function pointer
 */
extern void threads_create(void *(*startRoutine)(void*));

/**
 * @brief  Register User thread data for all threads
 * @param  data  Reference to the user data structo
 * @param  func  Optional function pointer to copy data
 */
extern void threads_registerDataAll(
        ThreadUserData* data,
        threads_copyDataFunc func);

/**
 * @brief  Register User thread data for one thread
 * @param  threadId thread Id 
 * @param  data  Reference to the user data structo
 * @param  func  Optional function pointer to copy data
 */
extern void threads_registerDataThread(
        int threadId,
        ThreadUserData* data,
        threads_copyDataFunc func);

/**
 * @brief  Register User thread data for a thread group
 * @param  groupId  group Id
 * @param  data  Reference to the user data structo
 * @param  func  Optional function pointer to copy data
 */
extern void threads_registerDataGroup(
        int groupId,
        ThreadUserData* data,
        threads_copyDataFunc func);

/**
 * @brief  Join the threads and free memory of data structures
 * @param
 */
extern void threads_destroy(void);

/**
 * @brief  Create Thread groups
 * @param  numberOfGroups The number of groups to create
 */
extern void threads_createGroups(int numberOfGroups);

#endif /* THREADS_H */
