#include "el.h"
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

//超时事件
event *tevent;


int time_from_start_to_now(struct timespec start) {
  struct timespec current;
  reset_time(&current);
  return (int)((current.tv_nsec - start.tv_nsec) / 1000000);
}

void reset_time(struct timespec *ts) {
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  (*ts).tv_sec = mts.tv_sec;
  (*ts).tv_nsec = mts.tv_nsec;

#else
  clock_gettime(CLOCK_REALTIME, ts);
#endif
}
