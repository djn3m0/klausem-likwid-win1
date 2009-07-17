/*
 * ===========================================================================
 *
 *       Filename:  cpuid.h
 *
 *    Description:  Header File cpuid Module. 
 *                  Reads out cpuid information and initilaizes a global 
 *                  data structure cpuid_info.
 *
 *        Version:  1.0
 *        Created:  07/15/2009
 *       Revision:  none
 *
 *         Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *        Company:  RRZE Erlangen
 *        Project:  none
 *      Copyright:  Copyright (c) 2009, Jan Treibig
 *
 * ===========================================================================
 */

/** \file cpuid.h
 * Header of cpuid module
 *
 */
#ifndef CPUID_H
#define CPUID_H
#include <types.h>

#define PENTIUM_M 0x0DU
#define CORE_DUO  0x0EU
#define CORE2_65  0x0FU
#define CORE2_45  0x17U
#define NEHALEM   0x1AU
#define XEON_MP   0x1DU
#define BARCELONA 0x02U
#define SHANGHAI  0x04U
#define ISTANBUL  0x08U

#define  P6_FAMILY        0x6U
#define  NETBURST_FAMILY  0xFU
#define  K10_FAMILY       0x10U

/** Structure holding cpuid information
 *
 */
extern CpuInfo cpuid_info;

/** Init routine to intialize global structure.
 *
 *  Determines: 
 *  - cpu family
 *  - cpu model
 *  - cpu stepping
 *  - cpu clock
 *  - Instruction Set Extension Flags
 *  - Performance counter features (Intel P6 only)
 *
 */
extern void cpuid_init (void);

#endif /*CPUID_H*/
