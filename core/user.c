#include "el.h"

el_loop *el_loop_new() {
  return loop_create();
}

event *el_event_new(int fd, int flags, cb_func cb, void *arg) {
  set_nonblock(fd); //must be no block io
  return event_init(fd, flags, cb, arg);
}

void el_event_add(el_loop *loop, event *e) {
  event_list_put(loop->active_events, e);
  loop->event_count++;
  //kqueue_add(loop, e);
  loop->io.add(loop, e);
}

int el_loop_run(el_loop *loop) {
  return loop_run(loop);
}
