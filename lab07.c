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
void listen_for_connections(int sockfd, int port);
int accept_connection(int sockfd);
void handle_client(int client_socket);
void print_ipv4_address(struct sockaddr_in *addr);

int main(int argc, char *argv[]) {
  int port = read_port_from_file();

  int sockfd = create_socket(port);
  bind_socket(sockfd, port);
  listen_for_connections(sockfd, port);

  while (1) {
    int client_socket = accept_connection(sockfd);
    handle_client(client_socket);
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

void listen_for_connections(int sockfd, int port) {
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

  // printf(":%d\n", ntohs(client_addr.sin_port));

  return client_socket;
}

void handle_client(int client_socket) {
  char buffer[MAX_BUFFER_SIZE] = {0};

  // Receive data from the client
  ssize_t recv_val = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
  if (recv_val < 0) {
    perror("recv");
    close(client_socket);
    exit(EXIT_FAILURE);
  }

  // Send a response (PONG for any message)
  const char *response = "PONG\n";
  ssize_t send_val = send(client_socket, response, strlen(response), 0);
  if (send_val < 0) {
    perror("send");
  }

  // Close the connection
  close(client_socket);
}

void print_ipv4_address(struct sockaddr_in *addr) {
  unsigned char *ip_bytes = (unsigned char *)&(addr->sin_addr.s_addr);
  printf("%u.%u.%u.%u", ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3]);
}
