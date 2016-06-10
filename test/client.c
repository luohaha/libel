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
  while (fgets(buf, MAX, stdin) != NULL) {
    if (write(sockfd, buf, strlen(buf)) < 0) {
      fprintf(stderr, "write error!\n");
      exit(-1);
    }
  }
  close(sockfd);
  return 0;
}
