#ifndef EL_H
#define EL_H

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<errno.h>
#include<fcntl.h>
#include <unistd.h>
#include "config.h"
#ifdef HAVE_KQUEUE_H
  #include <sys/event.h>
#endif
#ifdef HAVE_EPOLL_H
  #include <sys/epoll.h>
#endif

#define LISTENQ 1024
#define TRUE 1
#define FALSE 0
/*
---------------
all kinds of io multi flags
 */
#ifdef HAVE_KQUEUE_H
  #define READ_EVENT EVFILT_READ
  #define WRITE_EVENT EVFILT_WRITE
  //max kqueue event's size
  #define MAX_EVENT_COUNT 1024
#endif
#ifdef HAVE_EPOLL_H
  #define EPOLL_FD_SIZE 1024
#endif
//---------------------

//max buffer's size
#define MAXLINE 1024
//callback
typedef void (*cb_func) (int fd, int size, void *arg);
typedef void (*io_init) (struct __el_loop *);
typedef void (*io_add) (struct __el_loop *,  struct __event *);
typedef void (*io_del) (struct __el_loop *,  struct __event *);
typedef void (*io_dispatch) (struct __el_loop *);

/**
   事件
 **/
typedef struct __event {
  int fd;
  int flags;
  int size;
  void *arg;
  cb_func cb;
  struct __event *next;
  struct __event *prev;
} event;

/**
   事件队列
**/
typedef struct __el_event_list {
  int count;
  event *head;
} el_event_list;

typedef struct __io_multi {
  const char *name;
  io_init init;
  io_add add;
  io_del del;
  io_dispatch dispatch;
} io_multi;

typedef struct __el_loop {
  int event_count;
  io_multi io;
  int ioid;
  el_event_list *active_events;
  el_event_list *ready_events;
} el_loop;

//error.c
void error(const char *msg);

//user.c

el_loop *el_loop_new();
event *el_event_new(int fd, int flags, cb_func cb, void *arg);
void el_event_add(el_loop *loop, event *e);
int el_loop_run(el_loop *loop);

//event.c

event *event_init(int fd, int flags, cb_func cb, void *arg);
void event_free(event *ev);

//event_list.c

el_event_list *event_list_init();
void event_list_put(el_event_list *list, event *item);
event *event_list_get(el_event_list *list);
event *event_list_delete(el_event_list *list, int fd);
int event_list_size(el_event_list *list);
int event_list_is_empty(el_event_list *list);
void event_list_free(el_event_list *list);

//loop.c

el_loop *loop_create();
void loop_free(el_loop *loop);
int loop_run(el_loop *loop);

//kqueue.c

void kqueue_init(el_loop *loop);
void kqueue_add(el_loop *loop, event *ev);
void kqueue_del(el_loop *loop, event *ev);
void kqueue_dispatch(el_loop *loop);
void using_kqueue(el_loop* loop);

#endif
