#include <osdep/affinitymask.h>

void AffinityMask_clear(AffinityMask *mask)
{
#ifdef WIN32
	*mask = 0;
#else
	CPU_ZERO(&mask);
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

void AffinityMask_insertProcessorId(AffinityMask *mask, int processorId)
{
#ifdef WIN32
	*mask |= (1 << processorId);
#else
	CPU_SET(processorId, mask);
#endif
}
