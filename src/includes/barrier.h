/**
 *       @file  barrier.h
 *      @brief  Barrier Module Header file
 *
 * Detailed description starts here.
 *
 *     @author  Jan Treibig (jt), jan.treibig@gmail.com
 *
 *   @internal
 *     Created  08/03/2009
 *    Revision  ---
 *    Compiler  gcc
 *     Company  RRZE Erlangen
 *   Copyright  Copyright (c) 2009, Jan Treibig
 *
 * This source code is released for free distribution under the terms of the
 * GNU General Public License as published by the Free Software Foundation.
 * ============================================================================
 */
#ifndef BARRIER_H
#define BARRIER_H

#include <types.h>

/**
 * @brief  Initialize the barrier module
 * @param  numberOfThreads The total number of threads in the barrier
 */
extern void barrier_init(int numberOfGroups);

/**
 * @brief  Register a thread for a barrier
 * @param  threadId The id of the thread to register
 */
extern int barrier_registerGroup(int numThreads);
extern void barrier_registerThread(BarrierData* barr, int groupsId, int threadId);

/**
 * @brief  Synchronize threads
 * @param  threadId The id of the calling thread
 * @param  numberOfThreads Total number of threads in the barrier
 */
extern void  barrier_synchronize(BarrierData* barr);


#endif /*BARRIER_H*/
