#ifndef THREADID_H
#define THREADID_H

#ifdef WIN32
	#include <Windows.h>
	typedef HANDLE ThreadId;
#else
	#include <sys/types.h>
	typedef pthread_t ThreadId;
#endif

#endif
