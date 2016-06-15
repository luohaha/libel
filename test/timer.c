#include<libel/el.h>

int tt;

void cb(int fd, int size, void *arg) {
  el_loop *loop = (el_loop*) arg;
  printf("%d\n", tt++);
  event *e = el_timer(3000, cb, loop);
  el_event_add(loop, e);
}

void cb2(int fd, int size, void *arg) {
  printf("sigint\n");
}

int main() {
  el_loop *loop = el_loop_new();
  event *e = el_timer(3000, cb, loop);
  el_event_add(loop, e);
  event *e2 = el_sigevent_new(SIGINT, cb2, NULL);
  el_event_add(loop, e2);
  return el_loop_run(loop);
}
