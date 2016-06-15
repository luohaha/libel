#include "el.h"

void set_nonblock(int fd) {
  int flag;
  if ((flag = fcntl(fd, F_GETFL, 0)) < 0)
    error("fcntl get error!");
  if (fcntl(fd, F_SETFL, flag | O_NONBLOCK) < 0)
    error("fcntl set error!");
}

void init_pipe(sig_event *se) {
  if (pipe(se->pipe) < 0) {
    error("init pipe error!");
  }
}

void register_handler(int signo) {
  char c;
  sig_event *se = find_sigevent(&sig_list, signo);
  if (se == NULL)
    return;
  if (write(se->pipe[1], &c, 1) < 0)
    error("pipe fd write error!");
  close(se->pipe[1]);
  free(se);
}

void init_signal(sig_event * se) {
  if (signal(se->signo, register_handler) == SIG_ERR)
    error("init signal error!");
}
