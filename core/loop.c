#include "el.h"

el_loop *loop_create() {
  el_loop *loop = (el_loop*) malloc(sizeof(el_loop));
  sig_event_list_init(&sig_list);
  loop->event_count = 0;
  loop->timeout = -1;
  loop->active_events = event_list_init();
  loop->ready_events = event_list_init();
  //kqueue_init(loop);
  #ifdef HAVE_KQUEUE_H
    using_kqueue(loop);
  #endif
  #ifdef HAVE_EPOLL_H
    using_epoll(loop);
  #endif
  loop->io.init(loop);
  return loop;
}

void loop_free(el_loop *loop) {
  int nactive = event_list_size(loop->active_events);
  int nready = event_list_size(loop->ready_events);
  int i;
  for (i = 0; i < nactive; i++) {
    event *each = event_list_get(loop->active_events);
    event_free(each);
  }
  event_list_free(loop->active_events);
  for (i = 0; i < nready; i++) {
    event *each = event_list_get(loop->ready_events);
    event_free(each);
  }
  event_list_free(loop->ready_events);
  free(loop);
  return;
}

/**
   core part of all code
**/
int loop_run(el_loop *loop) {
  
  while (loop->event_count > 0) {
    loop->io.dispatch(loop);
    while (event_list_size(loop->ready_events) > 0) {
      event *e = event_list_get(loop->ready_events);
      //call the callback function
      e->cb(e->fd, e->size, e->arg);
      event_free(e);
      if (e->type == SIGNAL)
	close(e->fd);
      loop->event_count--;
    }
  }
  return 0;
}
