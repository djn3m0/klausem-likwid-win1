/*
 * ===========================================================================
 *
 *       Filename:  likwid.h
 *
 *    Description:  Header File of likwid marker API
 *
 *        Version:  1.0
 *        Created:  12/10/2009
 *       Revision:  none
 *
 *         Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *        Company:  RRZE Erlangen
 *        Project:  none
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

#ifndef LIKWID_H
#define LIKWID_H

#include <stdint.h>

#define LIKWID_INIT \
    CycleData _likwid_cycles; \
    timer_init()

#define LIKWID_START(coreId) \
    perfmon_markerStartCounters((coreId));  \
    timer_startCycles(&_likwid_cycles)

#define LIKWID_STOP(coreId) \
    timer_stopCycles(&_likwid_cycles)                            \
    perfmon_markerSetCycles(timer_printCycles(&_likwid_cycles);  \
    perfmon_markerStopCounters((coreId)

extern void timer_init(void );
extern void timer_startCycles(CyclesData* cycles);
extern void timer_stopCycles(CyclesData* cycles);
extern uint64_t timer_printCycles(CyclesData* cycles);

extern void perfmon_markerStartCounters(int core_id);
extern void perfmon_markerStopCounters(int core_id);
extern void perfmon_markerSetCycles(uint64_t cycles);

#endif /*LIKWID_H*/
