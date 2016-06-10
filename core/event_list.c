#include "el.h"

/**
   init event list
 **/
el_event_list *event_list_init() {
  event *new_event = (event*) malloc(sizeof(event));
  new_event->fd = -1;
  new_event->flags = 0;
  new_event->size = 0;
  new_event->arg = NULL;
  new_event->cb = NULL;
  new_event->next = new_event;
  new_event->prev = new_event;
  el_event_list *list = (el_event_list*) malloc(sizeof(el_event_list));
  list->count = 0;
  list->head = new_event;
  return list;
}

void event_list_put(el_event_list *list, event *item) {
  list->count++;
  item->prev = list->head->prev;
  list->head->prev->next = item;
  item->next = list->head;
  list->head->prev = item;
}

event *event_list_get(el_event_list *list) {
  if (list->count <= 0)
    error("event list is empty!");
  list->count--;
  event *ret = list->head->next;
  list->head->next = ret->next;
  ret->next->prev = list->head;
  return ret;
}

event *event_list_delete(el_event_list *list, int fd) {
  event *each = list->head;
  int size = list->count;
  int i;
  event *ret;
  for (i = 0; i < size; i++) {
    if (each->next->fd == fd) {
      ret = each->next;
      ret->next->prev = each;
      each->next = ret->next;
      break;
    }
    each = each->next;
  }
  if (i == size)
    error("event-list not contain item!");
  list->count--;
  return ret;
}

int event_list_size(el_event_list *list) {
  return list->count;
}

int event_list_is_empty(el_event_list *list) {
  return (list->count > 0) ? FALSE : TRUE;
}

void event_list_free(el_event_list *list) {
  free(list->head);
  free(list);
}
