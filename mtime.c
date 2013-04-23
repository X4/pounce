#include <stdlib.h>
#include <sys/time.h>

#include "mtime.h"

mtime_t mtime()
{
  struct timeval tv;
  
  (void) gettimeofday(&tv, NULL);
  return (long long) tv.tv_sec * 1000000 + (long long) tv.tv_usec;
}
