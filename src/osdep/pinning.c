#include <Windows.h>
#include <stdio.h>

#ifdef WIN32
static int
getProcessorID(DWORD_PTR* processAffinityMask)
{
    int processorId;

    for (processorId=0;processorId<24;processorId++){
        if (*processAffinityMask & (1 << processorId))
        {
            return processorId;
        }
    }
    fprintf(stderr, "Could not determine processor id from process affinity mask\n");
	return 25;
}
#else
static int
getProcessorID(cpu_set_t* processAffinityMask)
{
    int processorId;

    for (processorId=0;processorId<24;processorId++){
        if (CPU_ISSET(processorId,processAffinityMask))
        {
            return processorId;
        }
    }
    fprintf(stderr, "Could not determine processor id from process affinity mask\n");
	return 25;
}
#endif

int  pinning_processGetProcessorId()
{
#ifdef WIN32
	DWORD_PTR processAffinityMask;
	DWORD_PTR systemAffinityMask;

	BOOL res = GetProcessAffinityMask(
	  GetCurrentProcess(),
	  &processAffinityMask,
	  &systemAffinityMask
	);
	if (res == 0) {
		perror("GetCurrentProcess");
		exit(1);
	}
	return getProcessorID(&processAffinityMask);
#else
    cpu_set_t processAffinityMask;
    CPU_ZERO(&processAffinityMask);
    sched_getaffinity(getpid(),sizeof(cpu_set_t), &processAffinityMask);

    return getProcessorID(&processAffinityMask);
#endif
}


int  pinning_threadGetProcessorId()
{
#ifdef WIN32
	DWORD_PTR
		tmpThreadAffinityMask = (1 << 0),
		tmpThreadAffinityMask1 = -1;

	// temporarily set the thread affinity mask and retrieve old affinity mask
	DWORD_PTR origThreadAffinityMask = SetThreadAffinityMask(
		GetCurrentThread(),
		tmpThreadAffinityMask
	);
	if (origThreadAffinityMask == 0) {
		perror("SetThreadAffinityMask, used to get the thread affinity mask (1)");
		exit(1);
	}
	// reset the thread affinity mask to its original value
	tmpThreadAffinityMask1 = SetThreadAffinityMask(
		GetCurrentThread(),
		origThreadAffinityMask
	);
	if (tmpThreadAffinityMask1 == 0) {
		perror("SetThreadAffinityMask, used to get the thread affinity mask (2)");
		exit(1);
	}
	// check whether the resetting succeeded
	if (tmpThreadAffinityMask1 != tmpThreadAffinityMask) {
		perror("SetThreadAffinityMask, failed to reset thread affinity mask");
		exit(1);
	}
	return getProcessorID(&origThreadAffinityMask);
#else
    cpu_set_t  cpu_set;
    CPU_ZERO(&cpu_set);
    sched_getaffinity(gettid(),sizeof(cpu_set_t), &cpu_set);

    return getProcessorID(&cpu_set);
#endif
}


int  pinning_pinThread(int processorId)
{
#ifdef WIN32
	DWORD_PTR origThreadAffinityMask = SetThreadAffinityMask(
		GetCurrentThread(),
		1 << processorId
	);
	if (origThreadAffinityMask == 0) {
		perror("SetThreadAffinityMask");
		return FALSE;
	}
	return TRUE;
#else
	cpu_set_t cpuset;
    pthread_t thread;

    thread = pthread_self();
    CPU_ZERO(&cpuset);
    CPU_SET(processorId, &cpuset);
    if (pthread_setpinning_np(thread, sizeof(cpu_set_t), &cpuset)) 
    {
        perror("pthread_setpinning_np failed");
        return FALSE;
    }

    return TRUE;
#endif
}


int  pinning_pinProcess(int processorId)
{
#ifdef WIN32
	BOOL r = SetProcessAffinityMask(
		GetCurrentProcess(),
		1 << processorId
	);
	if (r == 0) {
		perror("SetProcessAffinityMask");
		return FALSE;
	}
	return TRUE;
#else
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(processorId, &cpuset);
	if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) == -1) 
	{
		perror("sched_setaffinity failed");
		/*
		TODO: take this error treatment or the one above?
		if (errno == EFAULT) 
        {
            fprintf(stderr, "A supplied memory address was invalid\n");
            exit(EXIT_FAILURE);
        }
        else if (errno == EINVAL) 
        {
            fprintf(stderr, "Processor is not available\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            fprintf(stderr, "Unknown error\n");
            exit(EXIT_FAILURE);
        }
		*/
        return FALSE;
	}

    return TRUE;
#endif
}
