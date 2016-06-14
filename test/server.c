#include<libel/el.h>

/**
create listener
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

void onread(int fd, int size, void *arg) {
  el_loop *loop = (el_loop*)arg;
  char buf[MAXLINE];
  int n;
  while ((n = read(fd, buf, MAXLINE)) > 0) {
    buf[n] = '\0';
    printf("%s", buf);
  }
  if (n == 0) {
    close(fd);
  } else if (n < 0) {
    if (errno == EWOULDBLOCK || errno == EAGAIN) {
      event *e = el_event_new(fd, READ_EVENT, onread, loop);
      el_event_add(loop, e);
      return;
    } else
      el_error("read from connected socket error!");
  }
}

void onaccept(int fd, int size, void *arg) {
  el_loop *loop = (el_loop*)arg;
  int i;
  for (i = 0; i < size; i++) {
    int connfd;
    if (( connfd = accept(fd, NULL, NULL)) < 0) {
      if (errno == EWOULDBLOCK || errno == ECONNABORTED
	  || errno == EINTR || errno == EPROTO) {
	continue;
      } else
	el_error("accept error!");
    }
    event *e = el_event_new(connfd, READ_EVENT, onread, loop);
    el_event_add(loop, e);
    event *old = el_event_new(fd, READ_EVENT, onaccept, loop);
    el_event_add(loop, old);
  }
}

int main() {
  int listenfd = create_listener();
  
  el_loop *loop = el_loop_new();
  event *e = el_event_new(listenfd, READ_EVENT, onaccept, loop);
  el_event_add(loop, e);
  return el_loop_run(loop);
}
