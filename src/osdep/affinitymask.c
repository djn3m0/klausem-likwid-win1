#include <osdep/affinitymask.h>
#include <stdio.h>

void AffinityMask_clear(AffinityMask *mask)
{
#ifdef WIN32
	*mask = 0;
#else
	CPU_ZERO(mask);
#endif
}

void AffinityMask_insert(AffinityMask *mask, int processorId)
{
#ifdef WIN32
	*mask |= (1 << processorId);
#else
	CPU_SET(processorId, mask);
#endif
}

int AffinityMask_contains(const AffinityMask *mask, int processorId)
{
#ifdef WIN32
	return (*mask & (1 << processorId)) != 0;
#else
	return CPU_ISSET(processorId, mask);
#endif
}

void AffinityMask_print(AffinityMask *mask)
{
	int procId=0;
	fprintf(stderr, "\n");
	for(procId=25; procId>=0; procId--) {
		if (AffinityMask_contains(mask, procId)) {
			fprintf(stderr, "1");
		} else {
			fprintf(stderr, "0");
		}
	}
	fprintf(stderr, "\n");
}
