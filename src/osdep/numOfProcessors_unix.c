#include <osindep/numOfProcessors.h>

uint32_t numOfProcessors() {
	FILE* pipe;

	uint32_t numProcs = -1;

	/* First determine the number of processors accessible */
	pipe = popen("cat /proc/cpuinfo | grep ^processor | wc -l", "r");
	if (fscanf(pipe, "%u\n", &numProcs) != 1)
	{
		fprintf(stderr, "Failed to fscanf cpuinfo!\n");
		exit(EXIT_FAILURE);
	}
	
	if (pclose(pipe) == -1) {
		fprintf(stderr, "Failed to close pipe for cpuinfo!\n");
		exit(EXIT_FAILURE);
	}

	return numProcs;
}
