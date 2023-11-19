#include <netdb.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {

	struct addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

char *readport(char *port) {

	file *File = fopen("port.text", "r");

	if(file == NULL) {

		printf("error opening");
		exit(1);
	}
}

char buffer(256);
char *port = fgets(buffer,256,file);

	struct addrinfo *results;
	char  *port = readPort("./port.txt");
	int e = getaddrinfo("localhost", port, &hints, &results);

	if(e != 0) {
		printf("gettaddrinfo: %s\n", gai_strerror(e));
		exit(-1);
	}

	struct addrinfo *r;
	int fd;
	for(r = results; r != NULL; r = r->ai_next) {
		fd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
		if(fd == -1) 
			continue;
		if(connect(fd, r->ai_addr, r-> ai_addrlen) != -1)
			continue;
		close(fd);
	}
	freeaddrinfo(results);

	if(r == NULL) {
		printf("Could not connect\n");
		exit(-1);
	}

	

	
	
}
