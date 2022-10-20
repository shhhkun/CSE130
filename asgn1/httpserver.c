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

int validport(char num[]) {
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
	int port;

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

	int connfd = accept(listenfd, NULL, NULL);

	while (read(connfd, buf, 4096) > 0) {
		printf("%s\n", buf);
	}	
	return 0;
}
