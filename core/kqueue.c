#include "el.h"

#ifdef HAVE_KQUEUE_H
/**
regist event
 **/
int regist(int epollfd, int fd, int type) {
  struct kevent changes[1];
  EV_SET(&changes[0], fd, type, EV_ADD, 0, 0, NULL);
  int ret = kevent(epollfd, changes, 1, NULL, 0, NULL);
  return TRUE;
}

/*
  delete event
*/
int delete(int epollfd, int fd, int flags) {
  struct kevent changes[1];
  EV_SET(&changes[0], fd, flags, EV_DELETE, 0, 0, NULL);
  int ret = kevent(epollfd, changes, 1, NULL, 0, NULL);
  return TRUE;
}

/**
   init kqueue
**/
void kqueue_init(el_loop *loop) {
  loop->ioid = kqueue();
  if (loop->ioid < 0)
    error("kqueue create error!\n");
}

void kqueue_add(el_loop *loop, event *ev) {
  regist(loop->ioid, ev->fd, ev->flags);
}

void kqueue_del(el_loop *loop, event *ev) {
  delete(loop->ioid, ev->fd, ev->flags);
}

void kqueue_dispatch(el_loop *loop) {
  struct kevent events[MAX_EVENT_COUNT];
  int ret;
  if (loop->timeout > 0) {
    //启动了定时器
    struct timespec t;
    if (loop->timeout >= 1000) {
      t.tv_nsec = 0;
      t.tv_sec = loop->timeout / 1000;
    } else {
      t.tv_nsec = loop->timeout * 1000000;
      t.tv_sec = 0;
    }
    ret = kevent(loop->ioid, NULL, 0, events, MAX_EVENT_COUNT, &t);
    if (ret == 0) {
      //timeout
      loop->timeout = -1;
      if (tevent != NULL)
	event_list_put(loop->ready_events, tevent);
    } 
  } else {
    ret = kevent(loop->ioid, NULL, 0, events, MAX_EVENT_COUNT, NULL);
  }
  
  int i;
  for (i = 0; i < ret; i++) {
    int sock = events[i].ident;
    int data = events[i].data;
    event *e = event_list_delete(loop->active_events, sock);
    delete(loop->ioid, e->fd, e->flags);
    e->size = data;
    event_list_put(loop->ready_events, e);
  }
}

void using_kqueue(el_loop *loop) {
  loop->io.name = "kqueue";
  loop->io.init = kqueue_init;
  loop->io.add = kqueue_add;
  loop->io.del = kqueue_del;
  loop->io.dispatch = kqueue_dispatch;
}

#endif
