cat > .test.h <<'EOM'
#include <sys/epoll.h>
EOM
if gcc -E .test.h
 then
  echo '#define HAVE_EPOLL_H 1' >> config.h
 else 
  echo '#ifdef HAVE_EPOLL_H' >> config.h
  echo '# undef HAVE_EPOLL_H' >> config.h
  echo '#endif' >> config.h
 fi

rm .test.h

cat > .test.h <<'EOM'
#include<sys/event.h>
EOM
if gcc -E .test.h
 then
  echo '#define HAVE_KQUEUE_H 1' >> config.h
 else 
  echo '#ifdef HAVE_KQUEUE_H' >> config.h
  echo '# undef HAVE_KQUEUE_H' >> config.h
  echo '#endif' >> config.h
 fi

rm .test.h
