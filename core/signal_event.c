#include "el.h"

//全局信号链表
sig_event_list sig_list;

sig_event *sig_event_init(event *e, int signo) {
  sig_event *se = (sig_event*) malloc(sizeof(sig_event));
  init_pipe(se->pipe);
  se->signo = signo;
  se->sevent = e;
  init_signal(se);
  sig_event_list_add(&sig_list, se);
  e->type = SIGNAL;
  return se;
}

void sig_event_free(sig_event *sig) {
  free(sig);
}

void sig_event_list_init(sig_event_list *list) {
  list->head = (sig_event*) malloc(sizeof(sig_event));
  list->head->next = list->head;
  list->head->prev = list->head;
  list->count = 0;
}

void sig_event_list_add(sig_event_list *list, sig_event *e) {
  list->count++;
  e->next = list->head->next;
  list->head->next->prev = e;
  e->prev = list->head;
  list->head->next = e;
}

sig_event *find_sigevent(sig_event_list *list, int no) {
  int count = list->count;
  int i;
  sig_event *first = list->head;
  sig_event *ret;
  for (i = 0; i < count; i++) {
    if (first->next->signo == no) {
      ret = first->next;
      first->next = ret->next;
      ret->next->prev = first;
      return ret;
    }
    first = first->next;
  }
  return NULL;
}
