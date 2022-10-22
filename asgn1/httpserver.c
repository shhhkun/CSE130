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

char *phrase(int code) {
    switch (code) {
    case 200: return "OK";
    case 201: return "Created";
    case 400: return "Bad Request";
    case 403: return "Forbidden";
    case 404: return "Not Found";
    case 500: return "Internal Server Error";
    case 501: return "Not Implemented";
    }
    return NULL;
}

void response(int connfd, char *resp, char *body, int code, int size) {
    sprintf(resp, "HTTP/1.1 %d %s\r\nContent-Length: %d\r\n\r\n", code, phrase(code), size);
    if (body != NULL) {
        strcat(resp, body);
    }
    //printf("sending response\n");
    //printf("%s", resp);
    send(connfd, resp, strlen(resp), 0); // send to client
}

void get(char *file, int connfd, char *buf, char *body, int size, char *resp) {
    int fd = open(file, O_RDONLY);
    while (read(fd, buf, 4096) > 0) {
        strcat(body, buf);
        memset(&buf, 0, sizeof(buf));
    }
    printf("body: %s", body);
    printf("size: %d\n", size);
    printf("body len: %lu\n", strlen(body));
    close(fd);
    response(connfd, resp, body, 200, size); // 200 'OK'
}

void head() {
}

void put() {
}

int main(int argc, char *argv[]) {
    char buf[4096];
    char header[4096];
    char body[4096];
    char resp[4096];
    int port;
    int count;
    int n;
    char method[8], filename[20], vers[9];
    struct stat st;

    if (argc < 2) {
        printf("not enough args\n"); // change later
        exit(1);
    }

    if (validport(argv[1]) == 1) { // if port invalid
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
        int connfd = accept(listenfd, NULL, NULL); // wait for connection

        while ((count = recv(connfd, buf, 4096, 0)) > 0) {
            write(1, buf, count);
            strcat(header, buf); // concatenate buf (request) into header
        }

        // parse request
        sscanf(header, "%s %s %s%n", method, filename, vers, &n);
        memmove(filename, filename + 1, strlen(filename)); // remove '/' from pathname

	//write(connfd, method, 4096);
	//send(connfd, method, strlen(method), 0);

        /*
	printf("n = %d\n", n);
	printf("method: %s\n"
               "file: %s\n"
	       "vers: %s\n", method, filename, vers);
	printf("method len = %zu\n", strlen(method));
	printf("file len = %zu\n", strlen(filename));
	printf("vers len = %zu\n", strlen(filename));
	*/

        stat(filename, &st);
        int size = st.st_size;

        memset(&buf, 0, sizeof(buf));
        get(filename, connfd, buf, body, size, resp);
    }
    return 0;
}
