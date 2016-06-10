#include "el.h"

event *event_init(int fd, int flags, cb_func cb, void *arg) {
  event *e = (event*) malloc(sizeof(event));
  e->fd = fd;
  e->flags = flags;
  e->arg = arg;
  e->cb = cb;
  return e;
}

void event_free(event *ev) {
  free(ev);
}
