#ifdef WIN32

char *strtok_r(char *s1, const char *s2, char **lasts);

#else

#include <string.h>

#endif
