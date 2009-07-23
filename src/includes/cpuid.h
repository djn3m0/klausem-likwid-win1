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

/* Intel P6 */
#define PENTIUM_M 0x0DU
#define CORE_DUO  0x0EU
#define CORE2_65  0x0FU
#define CORE2_45  0x17U
#define NEHALEM   0x1AU
#define XEON_MP   0x1DU

/* AMD K10 */
#define BARCELONA 0x02U
#define SHANGHAI  0x04U
#define ISTANBUL  0x08U

/* AMD K8 */

#define OPTERON_DC_E  0x21
#define OPTERON_DC_F  0x41
#define ATHLON64_X2   0x43
#define ATHLON64_X2_F 0x4B
#define ATHLON64_F1   0x4F
#define ATHLON64_F2   0x5F
#define ATHLON64_X2_G 0x6B
#define ATHLON64_G1   0x6F
#define ATHLON64_G2   0x7F


#define  P6_FAMILY        0x6U
#define  NETBURST_FAMILY  0xFU
#define  K10_FAMILY       0x10U
#define  K8_FAMILY        0xFU

/** Structure holding cpuid information
 *
 */
extern CpuInfo cpuid_info;
extern CpuTopology cpuid_topology;

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

extern void cpuid_initTopology (void);
extern void cpuid_initCacheTopology (void);

#endif /*CPUID_H*/
