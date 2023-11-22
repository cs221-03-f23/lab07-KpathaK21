#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024

volatile sig_atomic_t stop_server = 0;

void handle_signal(int signal) {
  if (signal == SIGINT) {
    printf("Server shutting down...\n");
    stop_server = 1;
  }
}

int read_port_from_file();

int create_socket(int port);
void bind_socket(int sockfd, int port);
void listen_for_connections(int sockfd);
int accept_connection(int sockfd);
void handle_client(int client_socket, int sockfd);
void print_ipv4_address(struct sockaddr_in *addr);

int main(int argc, char *argv[]) {
  int port = read_port_from_file();

  int sockfd = create_socket(port);
  bind_socket(sockfd, port);
  listen_for_connections(sockfd);

  while (1) {
    int client_socket = accept_connection(sockfd);
    handle_client(client_socket, sockfd);
  }

  close(sockfd);

  return 0;
}

int read_port_from_file() {
  FILE *file = fopen("port.txt", "r");
  if (file == NULL) {
    perror("Error opening port file");
    exit(EXIT_FAILURE);
  }

  int port;
  fscanf(file, "%d", &port);
  fclose(file);

  return port;
}

int create_socket(int port) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  return sockfd;
}

void bind_socket(int sockfd, int port) {
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
}

void listen_for_connections(int sockfd) {
  if (listen(sockfd, 3) == -1) {
    perror("listen");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
}

int accept_connection(int sockfd) {
  struct sockaddr_in client_addr;
  socklen_t client_addrlen = sizeof(client_addr);
  int client_socket =
      accept(sockfd, (struct sockaddr *)&client_addr, &client_addrlen);

  if (client_socket == -1) {
    perror("accept");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  return client_socket;
}

void handle_client(int client_socket, int sockfd) {
  char buffer[MAX_BUFFER_SIZE];

  ssize_t recv_val = recv(client_socket, buffer, MAX_BUFFER_SIZE - 1, 0);
  // error handling
  if (recv_val < 0) {
    perror("recv");
    close(client_socket);
    exit(EXIT_FAILURE);
  }
  // Check if the received message is "ping"
  if (strncmp(buffer, "PING", 4) == 0) {
    // Send "pong" as a response
    const char *response = "PONG\n";
    ssize_t send_val = send(client_socket, response, strlen(response), 0);
    if (send_val < 0) {
      perror("send");
      close(client_socket);
      exit(EXIT_FAILURE);
    }
  } else if (strncmp(buffer, "ping", 4) == 0) {

    // Send "pong" as a response
    const char *response = "PONG\n";
    ssize_t send_val = send(client_socket, response, strlen(response), 0);
    if (send_val < 0) {
      perror("send");
      close(client_socket);
      exit(EXIT_FAILURE);
    }
  } else {
    // Send "INVALID" as a response for any other message
    const char *response = "INVALID\n";
    ssize_t send_val = send(client_socket, response, strlen(response), 0);
    if (send_val < 0) {
      perror("send");
      close(client_socket);
      exit(EXIT_FAILURE);
    }
  }

  close(client_socket);
}

void print_ipv4_address(struct sockaddr_in *addr) {
  unsigned char *ip_bytes = (unsigned char *)&(addr->sin_addr.s_addr);
  printf("%u.%u.%u.%u", ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3]);
}
