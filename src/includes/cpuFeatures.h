/*
 * ===========================================================================
 *
 *       Filename:  cpuFeatures.h
 *
 *    Description:  Header File of Module cpuFeatures.
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


/** \file cpuFeatures.h
 * Header of cpuFeatures Module
 *
 */
#ifndef CPUFEATURES_H
#define CPUFEATURES_H

#include <types.h>

/**  Prints the feature flags
 *
 * Print feature flags of cpu with id cpu.
 * \param[in] cpu  The cpu id in the Os context
 */
extern void cpuFeatures_print(int cpu);

/**  Enable Cpu feature
 *
 * Enable a CPU feature. This is limited to several 
 * prefetchers. This is only supported on Intel architectures
 * \param[in] cpu  The cpu id in the Os context
 * \param[in] type The CpuFeature to enable
 */
extern void cpuFeatures_enable(int cpu, CpuFeature type);

/**  Disable Cpu feature
 *
 * Disable a CPU feature. This is limited to several 
 * prefetchers. This is only supported on Intel architectures
 * \param[in] cpu  The cpu id in the Os context
 * \param[in] type The CpuFeature to enable
 */
extern void cpuFeatures_disable(int cpu, CpuFeature type);

#endif /*CPUFEATURES_H*/
