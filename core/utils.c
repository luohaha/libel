#include "el.h"

void set_nonblock(int fd) {
  int flag;
  if ((flag = fcntl(fd, F_GETFL, 0)) < 0)
    error("fcntl get error!");
  if (fcntl(fd, F_SETFL, flag | O_NONBLOCK) < 0)
    error("fcntl set error!");
}
