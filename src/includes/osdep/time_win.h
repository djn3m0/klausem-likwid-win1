#ifndef TIME_H_fgz783
#define TIME_H_fgz783

//#include <time.h>
// contains struct timeval
#include <windows.h>
//#include <assert.h>


struct timezone 
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};
 

struct timespec {
	time_t  tv_sec;  //seconds
	long    tv_nsec; //nanoseconds
};

int gettimeofday(struct timeval *tv, struct timezone *tz);

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp);

#endif
