#include<libel/el.h>

void cb(int fd, int size, void *arg) {
  printf("sigint\n");
}

void cb2(int fd, int size, void *arg) {
  printf("sigquit\n");
}

int main() {
  el_loop *loop = el_loop_new();
  event *e = el_sigevent_new(SIGINT, cb, NULL);
  el_event_add(loop, e);
  event *e2 = el_sigevent_new(SIGQUIT, cb2, NULL);
  el_event_add(loop, e2);
  return el_loop_run(loop);
}
