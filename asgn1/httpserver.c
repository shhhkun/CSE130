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
    send(connfd, (void *) resp, strlen(resp), 0);
    return;
}

void get(char *file, int connfd, char *buf, char *body, int size, char *resp) {
    int fd = open(file, O_RDONLY);
    while (read(fd, buf, 4096) > 0) {
        strcat(body, buf);
        memset(&buf, 0, sizeof(buf));
    }
    close(fd);
    response(connfd, resp, body, 200, size); // 200 'OK'
    return;
}

void head(int connfd, int size, char *resp) {
    response(connfd, resp, NULL, 200, size); // 200 'OK'
    return;
}

void put(char *file, int connfd, char *buf, char *header) {
    int content_len;
    int count;
    char *ptr = strstr(header, "Content-Length:");
    int code = 200; // 'OK'
    if (ptr != NULL) {
        if (access(file, F_OK) != -1) { // if file doesn't exist
            code = 201; // change response to 201 'Created'
        }
        sscanf(ptr, "Content-Length: %d", &content_len);
        printf("content_len = %d", content_len);

        int fd = open(file, O_CREAT | O_WRONLY | O_TRUNC);

        while ((count = read(connfd, buf, 4096)) > 0) { // read request body
            write(fd, buf, count); // write to replace/update file
            memset(&buf, 0, sizeof(buf));
        }
        close(fd);
    }
    // insert response
    return;
}

int main(int argc, char *argv[]) {
    char buf[4096];
    char header[4096];
    char body[4096];
    char resp[4096];
    int port;
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
    printf("port = %d\n", port);

    int listenfd = create_listen_socket(port);

    if (listenfd == -1) { // change later
        printf("port error\n");
        exit(1);
    }

    while (1) {
        int connfd = accept(listenfd, NULL, NULL); // wait for connection

        while ((read(connfd, buf, sizeof(char))) > 0) {
            write(1, buf, sizeof(char));
            strcat(header, buf); // concatenate buf (request) into header
            char *end = strstr(header, "\r\n\r\n"); // if end of request break
            if (end != NULL) {
                break;
            }
            memset(&buf, 0, sizeof(buf));
        }

        sscanf(header, "%s %s %s", method, filename, vers); // parse request
        memmove(filename, filename + 1, strlen(filename)); // remove '/' from pathname

        stat(filename, &st);
        int size = st.st_size; // get file size (in bytes)

        // handle request method
        if (strcmp(method, "GET") == 0 || strcmp(method, "get") == 0) {
            get(filename, connfd, buf, body, size, resp);
        } else if (strcmp(method, "HEAD") == 0 || strcmp(method, "head") == 0) {
            head(connfd, size, resp);
        } else if (strcmp(method, "PUT") == 0 || strcmp(method, "put") == 0) {
            put(filename, connfd, buf, header);
        }

        // reset buffers
        memset(&buf, 0, sizeof(buf));
        memset(&header, 0, sizeof(header));
        memset(&body, 0, sizeof(body));
        memset(&resp, 0, sizeof(resp));

        close(connfd);
    }
    return 0;
}
