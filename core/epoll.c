#include "el.h"

#ifdef HAVE_EPOLL_H
void epoll_init(el_loop *loop) {
  loop->ioid = epoll_create(EPOLL_FD_SIZE);
  if (loop->ioid < 0)
    error("epoll create error!");
}

void epoll_add(el_loop *loop, event *ev) {
  struct epoll_event event;
  event.data.fd = ev->fd;
  event.events = ev->flags | EPOLLET;
  int n = epoll_ctl(loop->ioid, EPOLL_CTL_ADD, ev->fd, &event);
  if (n < 0)
    error("epoll add error!");
}

void epoll_del(el_loop *loop, event *ev) {
  int n = epoll_ctl(loop->ioid, EPOLL_CTL_DEL, ev->fd, NULL);
  if (n < 0)
    error("epoll del error!");
}

void epoll_dispatch(el_loop *loop) {
  struct epoll_event events[EPOLL_FD_SIZE];
  int ret = epoll_wait(loop->ioid, events, EPOLL_FD_SIZE, loop->timeout);
  if (ret == 0) {
    //timeout
    loop->timeout = -1;
    if (tevent != NULL)
      event_list_put(loop->ready_events, tevent);
  }
  int i;
  for (i = 0; i < ret; i++) {
    int sock = events[i].data.fd;
    //int data = strlen((char*)events[i].data.ptr);
    event *e = event_list_delete(loop->active_events, sock);
    epoll_del(loop, e);
    e->size = 1;
    event_list_put(loop->ready_events, e);
  }
}

void using_epoll(el_loop *loop) {
  loop->io.name = "epoll";
  loop->io.init = epoll_init;
  loop->io.add = epoll_add;
  loop->io.del = epoll_del;
  loop->io.dispatch = epoll_dispatch;
}
#endif
