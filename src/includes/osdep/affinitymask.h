#ifndef AFFINITYMASK_H
#define AFFINITYMASK_H

#ifdef WIN32
	// in windows the affinity mask is a DWORD_PTR which means that we can take a size_t as well
	#include "stddef.h"
	typedef size_t AffinityMask;
#else
	#ifndef _GNU_SOURCE
		#define _GNU_SOURCE
	#endif
	#include <sched.h>
	typedef cpu_set_t AffinityMask;
#endif

void AffinityMask_clear(AffinityMask *mask);
void AffinityMask_insert(AffinityMask *mask, int processorId);
int AffinityMask_contains(const AffinityMask *mask, int processorId);

#endif
