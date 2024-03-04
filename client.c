#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_LENGTH 512

int sockfd;

void stop(int signal) {
  close(sockfd);
  printf("\ndisconnected\n");
  exit(0);
}

int main(int argc, char **argv) {

  if (argc < 3) {
    printf("%s: usage %s <host> <port>\n", argv[0], argv[0]);
    exit(1);
  }

  char *host = argv[1];
  int port = atoi(argv[2]);

  printf("connecting to %s:%d\n", host, port);

  struct sockaddr_in server_sai;

  if (0 > (sockfd = socket(AF_INET, SOCK_STREAM, 0))) {
    perror("socket");
    exit(1);
  }

  signal(SIGINT, stop);

  server_sai.sin_family = AF_INET;
  server_sai.sin_port = htons(port);
  server_sai.sin_addr.s_addr = inet_addr(host);

  if (0 > connect(sockfd, (struct sockaddr *)&server_sai, sizeof(server_sai))) {
    perror("connect");
    exit(1);
  }

  char buffer[BUFFER_LENGTH];

  int status = 0;
  int ret = 0;
  socklen_t len = sizeof(status);

  while (1) {
    printf("enter a message: ");
    fgets(buffer, BUFFER_LENGTH, stdin);

    int bytes = 0;
    if (-1 == (bytes = send(sockfd, buffer, strlen(buffer), 0))) {
      perror("send");
      exit(1);
    }

    if (0 != (ret = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &status, &len))) {
      printf("%s", strerror(ret));
    }

    if (status != 0) {
      close(sockfd);
      printf("connection lost\n");
      exit(0);
    }

    bzero(buffer, BUFFER_LENGTH);
  }
}
