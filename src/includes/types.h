#ifndef TYPES_H
#define TYPES_H


/* #####   HEADER FILE INCLUDES   ######################################### */
#include <stdint.h>

#include <strUtil_types.h>
#include <libperfctr_types.h>
#include <timer_types.h>
#include <tree_types.h>
#include <cpuid_types.h>
#include <cpuFeatures_types.h>
#include <asciiBoxes_types.h>
#include <asciiTable_types.h>
#include <perfmon_types.h>
#include <multiplex_types.h>


/* #####   EXPORTED MACROS   ############################################## */

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

#define TRUE  1
#define FALSE 0

#define HLINE "-------------------------------------------------------------\n"
#define SLINE "*************************************************************\n"

#define LLU_CAST  (unsigned long long)

#endif /*TYPES_H*/
