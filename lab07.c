#include <netdb.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024



int create_socket(const char *port);
void bind_socket(int sockfd, const char *port);
void listen_for_connections(int sockfd);
int accept_connection(int sockfd);
void handle_client(int client_socket);
void print_ipv4_address(struct sockaddr_in *addr);


int main() {

	struct addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	FILE *file = fopen("port.txt", "r");
	        if (file == NULL) {
	            perror("Error opening port.txt");
	            exit(1);
	        }
	
	    char buffer[256];
	    
	    if (fgets(buffer, sizeof(buffer), file) == NULL) {
	            fprintf(stderr, "Error reading from port.txt");
	            fclose(file);
	            exit(1);
	        }
	      
	        char *port = fgets(buffer, 256, file);
	        fclose(file);
	
	   

	    struct addrinfo *results;
	        int e = getaddrinfo("localhost", port, &hints, &results);
	    
	        if (e != 0) {
	            printf("getaddrinfo: %s\n", gai_strerror(e));
	            exit(-1);
	        }
	    
	        struct addrinfo *r;
	        int fd;

	    for (r = results; r != NULL; r = r->ai_next) {
	            fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
	            if (fd == -1)
	                continue;
	            if (connect(fd, r->ai_addr, r->ai_addrlen) != -1)
	                break; // Connected successfully
	            close(fd);
	        }
	    
	        freeaddrinfo(results);
	    
if (r == NULL) {
        printf("Could not connect\n");
        exit(-1);
    }

close(fd);

    return 0;
}

int create_socket(const char *port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

void bind_socket(int sockfd, const char *port) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(atoi(port));

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
    printf("Server listening on port %d...\n", ntohs(((struct sockaddr_in){.sin_port = 0}).sin_port));
}


    int accept_connection(int sockfd) {
        struct sockaddr_in client_addr;
        socklen_t client_addrlen = sizeof(client_addr);
        int client_socket = accept(sockfd, (struct sockaddr *)&client_addr, &client_addrlen);
    
        if (client_socket == -1) {
            perror("accept");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    
        printf("Accepted connection from ");
        print_ipv4_address(&client_addr);
        printf(":%d\n", client_addr.sin_port);  // Print the port directly in network byte order
    
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

       // Display the received message
            struct sockaddr_in client_addr;
            socklen_t client_addrlen = sizeof(client_addr);
            if (getpeername(client_socket, (struct sockaddr *)&client_addr, &client_addrlen) == 0) {
                // Extract and print individual bytes of the IPv4 address
                unsigned char *ip_bytes = (unsigned char *)&(client_addr.sin_addr.s_addr);
                printf("Received from server: %s\n", buffer);
                printf("Accepted connection from %u.%u.%u.%u:%d\n",
                       ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3],
                       ntohs(client_addr.sin_port));
            } else {
                perror("getpeername");
            }

            // Close the connection
                close(client_socket);
            }
void print_ipv4_address(struct sockaddr_in *addr) {
    unsigned char *ip_bytes = (unsigned char *)&(addr->sin_addr.s_addr);
    printf("%u.%u.%u.%u", ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3]);
}
