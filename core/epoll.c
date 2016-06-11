#include "el.h"

void epoll_init(el_loop *loop) {
  loop->ioid = epoll_create();
}
