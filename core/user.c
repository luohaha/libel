#include "el.h"

el_loop *el_loop_new() {
  return loop_create();
}

event *el_event_new(int fd, int flags, cb_func cb, void *arg) {
  set_nonblock(fd); //must be no block io
  return event_init(fd, flags, cb, arg);
}

event *el_sigevent_new(int signo, cb_func cb, void *arg) {
  event *e = event_init(-1, READ_EVENT, cb, arg);
  sig_event *se = sig_event_init(e, signo);
  e->fd = se->pipe[0];
  set_nonblock(e->fd);
  return e;
}

event *el_timer(int timeout, cb_func cb, void *arg) {
  tevent = event_init(timeout, READ_EVENT, cb, arg);
  tevent->type = TIMER;
  return tevent;
}

void el_event_add(el_loop *loop, event *e) {
  if (e->type == TIMER) {
    loop->timeout = e->fd;
    loop->event_count++;
  } else {
    event_list_put(loop->active_events, e);
    loop->event_count++;
    loop->io.add(loop, e);
  }
}

int el_loop_run(el_loop *loop) {
  return loop_run(loop);
}

void el_loop_free(el_loop *loop) {
  close(loop->ioid);
  loop_free(loop);
}

void el_error(const char *msg) {
  error(msg);
}
