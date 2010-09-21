#include <osdep/executePinned.h>
#include <bstrlib.h>
#include <types.h>
#include <errno.h>
#include <stdio.h>
#include <osdep/setenv.h>
#include <osdep/affinity.h>

#ifdef WIN32
#include <osdep/likCreateProcessWithDll_win.h>
#else
#include <osdep/execvp.h>
#endif

#ifdef COLOR
#include <textcolor.h>
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

int pinPid(int cpuid)
{
	int result = 0;
	AffinityMask defaultMask = affinity_getLargestProcessAffinityMask();

#ifdef COLOR
    color_on(BRIGHT, COLOR);
#endif

	printf("[likwid-pin] Main PID -> core %d - ",  cpuid);
	// reset process affinity mask so that thread affinity mask can be set
	if (result == 0 && affinity_setProcessAffinityMask(defaultMask) == FALSE) {
		printf("affinity_setProcessAffinityMask failed : %s \n",strerror(errno));
		result = -1;
	}
	// set thread affinity mask
	if (result == 0 && affinity_pinThread(cpuid) == FALSE) {
		printf("affinity_pinThread failed : %s \n",strerror(errno));
		result = -1;
	}
	if (result == 0) {
		printf("OK\n");
	}
#ifdef COLOR
    color_reset();
#endif
	return result;
}

int executePinned_launch(const char *argv[], const int threads[], int numThreads, int skipMask) {
    bstring  pinString;
    bstring  skipString;

	if (numThreads > 1)
	{
		int i;
		pinString = bformat("%d",threads[1]);

		for (i=2; i < numThreads;i++)
		{
			bformata(pinString,",%d",threads[i]);
		}

		skipString = bformat("%d",skipMask);
		setenv("LIKWID_PIN",(char*) pinString->data , 1);
		setenv("LIKWID_SKIP",(char*) skipString->data , 1);
#ifndef WIN32
		setenv("LD_PRELOAD",TOSTRING(LIBLIKWIDPIN), 1);
#endif
	}

	/* only newly created threads will be pinned, so the current thread must be pinned manually */
	if (pinPid(threads[0]) != 0) return -1;

    fflush(stdout);

#ifdef WIN32
	likCreateProcessWithDll("", argv[0], "detoured.dll", "executePinned_clientDll.dll");
#else
	execvp(argv[0], argv);
	perror("execvp");
	fprintf(stderr,"failed to execute %s\n", argv[0]);
#endif
	return EXIT_FAILURE;
}
