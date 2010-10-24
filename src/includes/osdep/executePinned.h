#include <osdep/threadid.h>

int executePinned_launch(const char *argv[], const int threads[], int numThreads, int skipMask);

int executePinned_pinNextThread(ThreadId threadId);
int executePinned_currentThreadId();
