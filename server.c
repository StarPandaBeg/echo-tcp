#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_LENGTH 512
#define MAX_CONNECTIONS 3

int sockfd;

void stop(int signal) {
  close(sockfd);
  printf("\nserver closed\n");
  exit(0);
}

void break_connection(int signal) { exit(0); }

int main(int argc, char **argv) {

  if (argc < 2) {
    printf("%s: usage %s <port>\n", argv[0], argv[0]);
    exit(1);
  }

  int port = atoi(argv[1]);

  int incoming_socket;
  struct sockaddr_in sai;

  socklen_t slen = sizeof(sai);

  char buffer[BUFFER_LENGTH];

  memset((char *)&sai, 0, slen);

  if (0 > (sockfd = socket(AF_INET, SOCK_STREAM, 0))) {
    perror("socket");
    exit(1);
  }

  signal(SIGINT, stop);

  sai.sin_addr.s_addr = INADDR_ANY;
  sai.sin_port = htons(port);
  sai.sin_family = AF_INET;

  if (0 > bind(sockfd, (struct sockaddr *)&sai, sizeof(sai))) {
    perror("bind");
    exit(1);
  }

  if (0 > listen(sockfd, MAX_CONNECTIONS)) {
    perror("listen");
    exit(1);
  }

  printf("server started on port %d\n", port);

  while (1) {
    if (0 >
        (incoming_socket = accept(sockfd, (struct sockaddr *)&sai, &slen))) {
      perror("accept");
      exit(1);
    }

    if (fork() == 0) {
      char buffer_ip[16];
      inet_ntop(AF_INET, &sai, buffer_ip, slen);
      printf("Incoming connection from %s\n", buffer_ip);

      signal(SIGINT, break_connection);

      memset(buffer, 0, BUFFER_LENGTH);

      int bytes = 0;
      while (0 != (bytes = recv(incoming_socket, buffer, BUFFER_LENGTH, 0))) {
        printf("[%s]: %s\n", buffer_ip, buffer);
        bzero(buffer, BUFFER_LENGTH);
      }

      printf("%s disconnected\n", buffer_ip);

      close(incoming_socket);
    }
  }
}
