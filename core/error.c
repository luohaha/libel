#include "el.h"
/**
   Yixin Luo
   handler error

 **/
/**
   print error msg and exit
*/
void error(const char *msg) {
  fprintf(stderr, "%s, error information : \n", msg);
  fprintf(stderr, "%s\n", (char*)strerror(errno));
  exit(-1);
}
