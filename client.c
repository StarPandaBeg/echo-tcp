#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_LENGTH 512

int sockfd;

void stop(int signal) {
  close(sockfd);
  printf("disconnected\n");
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

  while (1) {
    printf("enter a message: ");
    fgets(buffer, BUFFER_LENGTH, stdin);

    if (-1 == send(sockfd, buffer, strlen(buffer), 0)) {
      perror("send");
      exit(1);
    }
  }
}
