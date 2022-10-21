#include "bind.h"

#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <err.h>
#include <ctype.h>

int validport(char *num) {
    int i = 0;
    while (num[i] != '\0') {
        if (isdigit(num[i]) == 0) {
            return 1;
        }
        i += 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    char buf[4096];
    char header[4096];
    int port;
    int count;
    char method[8000], filename[8000], vers[8000];
    
    if (argc < 2) {
        printf("not enough args\n"); // change later
        exit(1);
    }

    if (validport(argv[1]) == 1) {
        port = 0;
    } else {
        port = strtol(argv[1], NULL, 10); // take port number
    }
    printf("port = %d\n", port); //

    int listenfd = create_listen_socket(port);

    if (listenfd == -1) {
        printf("port error\n");
        exit(1);
    }

    while (1) {
        int connfd = accept(listenfd, NULL, NULL);

        while ((count = recv(connfd, buf, 4096, 0)) > 0) {
		write(1, buf, count);
		strcat(header, buf); // concatenate buf (request) into header
        }
	sscanf(header, "%s %s %s", method, filename, vers);
	memmove(filename, filename + 1, strlen(filename));
	printf("method: %s\n"
               "file: %s\n"
	       "vers: %s\n", method, filename, vers);

	printf("method len = %zu\n", strlen(method));
	printf("file len = %zu\n", strlen(filename));
	printf("vers len = %zu\n", strlen(filename));
    }
    return 0;
}
