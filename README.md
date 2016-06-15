# libel
An event-driven library.  
Libel execute a callback function when a specific event happen on a file descriptor, or a sepcific signal happen. And also after a timeout has been reached.

###install

```
make config
make
make install
```

###use to use

After install libel, you need to add header: 

```
#include<libel/el.h>
```

You can compile your code using `-lel` like this:  

```
gcc -o test test.c -lel
```


###support

Libel is a multi-platform support library, and now support freeBSD, OS X and linux.  
Now libel use `kqueue` on freebsd(also OS X), and `epoll` on linux.

###struct

`el_loop`
>An event loop.

`event`
> An event.

###api

```c
el_loop *el_loop_new()
```
>create a new event loop.

```c
event *el_event_new(int fd, int flags, void (*cb) (int fd, int size, void *arg), void *arg)
```
>create a new event.  
>fd : the file descriptor that you want to listen.  
>flags : `READ_EVENT` or `WRITE_EVENT` or both  
>cb : callback function that will be called by libel  
>arg : it will be pass to callback function

```c
event *el_sigevent_new(int signo, cb_func cb, void *arg) 
```
>create a new signal handler event. `signo` is the signal type, such as `SIGINT`.

```c
event *el_timer(int timeout, cb_func cb, void *arg)
```
>create a new timer event, `timeout`'s unit is millisecond.

```c
void el_event_add(el_loop *loop, event *e)
```
>add an event to a loop.

```c
int el_loop_run(el_loop *loop)
```
> To start this event loop.

```c
void el_loop_free(el_loop *loop)
```
>free event loop.

```c
void el_error(const char *msg)
```
>print msg, errno message and exit(-1).

###example

#####1. An example of a simple server accept connection from many clients.

server  

```c
#include<libel/el.h>

/**
create a listener
 **/
int create_listener() {
  int listenfd;
  struct sockaddr_in *servaddr = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in));
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    el_error("socket error!");
  servaddr->sin_family = AF_INET;
  servaddr->sin_port = htons(3333);
  if (inet_pton(AF_INET, "0.0.0.0", &servaddr->sin_addr) < 0)
    el_error("inet_pton error!");
  if (bind(listenfd, (struct sockaddr *)servaddr, sizeof(struct sockaddr_in)) < 0)
    el_error("bind error!");
  if (listen(listenfd, LISTENQ) < 0)
    el_error("listen error!");
  return listenfd;
}

/**
	处理客户端发送过来的数据，打印数据。
*/
void onread(int fd, int size, void *arg) {
  el_loop *loop = (el_loop*)arg;
  char buf[MAXLINE];
  int n;
  while ((n = read(fd, buf, MAXLINE)) > 0) {
    buf[n] = '\0';
    printf("%s", buf);
  }
  if (n == 0) {
  //客户端已经完成数据的发送。关闭连接。
    close(fd);
  } else if (n < 0) {
    if (errno == EWOULDBLOCK || errno == EAGAIN) {
    //如果客户端未发送数据完成，需要继续监听
      event *e = el_event_new(fd, READ_EVENT, onread, loop);
      el_event_add(loop, e);
      return;
    } else
      el_error("read from connected socket error!");
  }
}

/*
	处理客户端发起连接的回调函数
*/
void onaccept(int fd, int size, void *arg) {
  el_loop *loop = (el_loop*)arg;
  int i;
  //size 为缓冲区中的字节数
  for (i = 0; i < size; i++) {
    int connfd;
    if (( connfd = accept(fd, NULL, NULL)) < 0) {
      if (errno == EWOULDBLOCK || errno == ECONNABORTED
	  || errno == EINTR || errno == EPROTO) {
	continue;
      } else
	el_error("accept error!");
    }
    /**
    	将新建立的连接新建为事件，并加入loop。回调函数为onread。
    */
    event *e = el_event_new(connfd, READ_EVENT, onread, loop);
    el_event_add(loop, e);
    /**
    	在libel的处理机制中，每次事件触发并处理完成后，都会从loop中删除，
    	所以需要重新创建，并加入loop
    */
    event *old = el_event_new(fd, READ_EVENT, onaccept, loop);
    el_event_add(loop, old);
  }
}

int main() {
  //创建监听
  int listenfd = create_listener();
  //新建loop
  el_loop *loop = el_loop_new();
  //新建要监听的事件，回调函数为 onaccept
  event *e = el_event_new(listenfd, READ_EVENT, onaccept, loop);
  //将要监听的事件注册到loop上
  el_event_add(loop, e);
  //启动loop
  return el_loop_run(loop);
}
```

client

```c
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<errno.h>
#include<string.h>
#define MAX 1024

int main() {
  int sockfd;
  struct sockaddr_in servaddr;
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "socket error!\n");
    exit(-1);
  }
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(3333);
  if (inet_pton(AF_INET, "0.0.0.0", &servaddr.sin_addr) < 0) {
    fprintf(stderr, "inet_pton error!\n");
    exit(-1);
  }
  if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "connect error!\n");
    exit(-1);
  }
  char buf[MAX];
  /**
  	客户端从终端接收数据，并向服务器端发送。
  */
  while (fgets(buf, MAX, stdin) != NULL) {
    if (write(sockfd, buf, strlen(buf)) < 0) {
      fprintf(stderr, "write error!\n");
      exit(-1);
    }
  }
  close(sockfd);
  return 0;
}

```

#####2. an example of signal handler

```c
#include<libel/el.h>

void cb(int fd, int size, void *arg) {
  printf("sigint\n");
}

void cb2(int fd, int size, void *arg) {
  printf("sigquit\n");
}

int main() {
  el_loop *loop = el_loop_new();
  //创建信号处理函数，处理信号SIGINT
  event *e = el_sigevent_new(SIGINT, cb, NULL);
  el_event_add(loop, e);
  //创建信号处理函数，处理信号SIGQUIT
  event *e2 = el_sigevent_new(SIGQUIT, cb2, NULL);
  el_event_add(loop, e2);
  return el_loop_run(loop);
}

```

#####3. an example of timer

```c
#include<libel/el.h>

int tt;

void cb(int fd, int size, void *arg) {
  el_loop *loop = (el_loop*) arg;
  printf("%d\n", tt++);
  //如果想要定时器继续，同样需要重新建立
  event *e = el_timer(3000, cb, loop);
  el_event_add(loop, e);
}

void cb2(int fd, int size, void *arg) {
  printf("sigint\n");
}

int main() {
  el_loop *loop = el_loop_new();
  //创建定时事件，时间间隔为3000ms
  event *e = el_timer(3000, cb, loop);
  el_event_add(loop, e);
  //创建信号处理事件，处理信号SIGINT
  event *e2 = el_sigevent_new(SIGINT, cb2, NULL);
  el_event_add(loop, e2);
  return el_loop_run(loop);
}

```

###Todo
`support more system`.