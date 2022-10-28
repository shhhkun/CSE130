#include "bind.h"

#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <ctype.h>
#include <libgen.h>

#define GET  1
#define HEAD 2
#define PUT  3

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

int methodtype(char *method) {
    if (strcmp(method, "GET") == 0 || strcmp(method, "get") == 0) {
        return GET;
    } else if (strcmp(method, "HEAD") == 0 || strcmp(method, "head") == 0) {
        return HEAD;
    } else if (strcmp(method, "PUT") == 0 || strcmp(method, "put") == 0) {
        return PUT;
    }
    return 0;
}

void response(int connfd, char *resp, char *body, int code, int size) {
    sprintf(resp, "HTTP/1.1 %d %s\r\nContent-Length: %d\r\n\r\n", code, phrase(code), size);
    if (body != NULL) {
        strcat(resp, body);
    }
    write(connfd, resp, strlen(resp));
    //send(connfd, resp, strlen(resp), 0);
    return;
}

void get(char *file, int connfd, char *buf, int size, char *resp) {
    int count;
    int fd = open(file, O_RDONLY); // change flag to RDWR and check if directory
    response(connfd, resp, NULL, 200, size); // 200 'OK'
    while ((count = read(fd, buf, 4096)) > 0) {
        write(connfd, buf, count);
    }
    close(fd);
    return;
}

void head(int connfd, int size, char *resp) {
    response(connfd, resp, NULL, 200, size); // 200 'OK'
    return;
}

void put(char *file, int connfd, char *buf, char *header) {
    int content_len;
    int fd, count, bytes_r = 0;

    char *ptr = strstr(header, "Content-Length:"); // move ptr to content len
    int code = 200; // default 200 'OK'
    if (ptr != NULL) {
        if (access(file, F_OK) == -1) { // if file doesn't exist
            fd = open(file, O_CREAT + O_WRONLY + S_IRUSR + S_IWUSR);
            code = 201; // change response to 201 'Created'
        } else { // if file does exist
            fd = open(file, O_TRUNC + O_WRONLY);
        }
        sscanf(ptr, "Content-Length: %d", &content_len);

        while ((count = read(connfd, buf, 4096)) > 0) { // read request body
            write(fd, buf, count); // write to replace/update file
            bytes_r += count; // bytes read
            if (content_len == bytes_r) {
                break;
            }
        }
        close(fd);

        if (code == 200) {
            response(connfd, buf, "OK\n", code, 3); // 200 'OK'
        } else {
            response(connfd, buf, "Created\n", code, 8); // 201 'Created'
        }
    }
    return;
}

int main(int argc, char *argv[]) {
    char buf[4096];
    char header[4096];
    char body[4096];
    char resp[4096];
    char hfield[4096];
    int port, errors = 0, not_rq = 1, malformed_header = 0; // 0 'false', 1 'true'
    char method[2048], filepath[2048], vers[2048];
    char key[500], value[500];
    struct stat st;

    if (argc < 2) {
        warnx("wrong arguments: %s port_num\n"
              "usage: %s <port>",
            argv[0], argv[0]);
        exit(1);
    }

    if (validport(argv[1]) == 1) { // if port invalid
        port = 0;
    } else {
        port = strtol(argv[1], NULL, 10); // take port number
    }

    int listenfd = create_listen_socket(port);

    if (listenfd == -3) {
        warnx("bind: %s", strerror(errno));
        exit(1);
    }

    while (1) {
        int connfd = accept(listenfd, NULL, NULL); // wait for connection

        errors = 0;
        not_rq = 1;

        while ((read(connfd, buf, sizeof(char))) > 0) {
            if (not_rq == 0) {
                strcat(hfield, buf);
            }
            strcat(header, buf); // concatenate buf (request) into header
            char *start = strstr(header, "\r\n"); // end of request-line
            char *mid = strstr(hfield, "\r\n"); // end of a header-field
            char *end = strstr(header, "\r\n\r\n"); // end of a request
            memset(&buf, 0, sizeof(buf));
            if (mid != NULL && end == NULL) { // found header-field
                int kv = sscanf(hfield, "%s %s", key, value);
                if (key[strlen(key) - 1] != ':' || kv != 2) {
                    malformed_header = 1;
                }
                memset(&hfield, 0, sizeof(hfield));
                memset(&key, 0, sizeof(key));
                memset(&value, 0, sizeof(value));
            }
            if (end != NULL) { // found end of request
                memset(&hfield, 0, sizeof(hfield));
                break;
            }
            if (start != NULL && not_rq == 1) { // found end of request-line
                not_rq = 0;
            }
        }

        sscanf(header, "%s %s %s", method, filepath, vers); // parse request
        //char path[4096] = ".";
        //strcat(path, filepath);
        //char *filename = realpath(path, NULL); // get absolute path
        char *filename = filepath + 1;
        int method_type = methodtype(method);

        stat(filename, &st);
        int size = st.st_size; // get file size (in bytes)

        if ((strcmp(vers, "HTTP/1.1") != 0 || method[0] == '\0' || filepath[0] == '\0'
                || vers[0] == '\0' || malformed_header == 1)
            && errors == 0) {
            response(connfd, buf, "Bad Request\n", 400,
                12); // if wrong HTTP vers, missing stuff, or malformed header-fields
            errors = 1;
            malformed_header = 0;
        }
        if ((access(filename, F_OK) == -1 && errors == 0) && method_type != PUT) {
            response(
                connfd, buf, "Not Found\n", 404, 10); // if file doesn't exist and not PUT request
            errors = 1;
        }
        if (access(filename, F_OK) == 0) { // first check that file exists
            if ((access(filename, R_OK) == -1 && errors == 0) && method_type != PUT) {
                response(
                    connfd, buf, "Forbidden\n", 403, 10); // if no read perms and not PUT request
                errors = 1;
            }
            if ((access(filename, W_OK) == -1 && errors == 0) && method_type == PUT) {
                response(
                    connfd, buf, "Forbidden\n", 403, 10); // if no write perms and is PUT request
                errors = 1;
            }
        }
        if (S_ISDIR(st.st_mode) != 0 && errors == 0) {
            response(connfd, buf, "Forbidden\n", 403, 10); // if filepath leads to directory
            errors = 1;
        }
        if (method_type != GET && method_type != HEAD && method_type != PUT && errors == 0) {
            response(connfd, buf, "Not Implemented\n", 501, 16); // if invalid method
            errors = 1;
        }

        // handle request method
        if (method_type == GET && errors == 0) {
            get(filename, connfd, buf, size, resp);
        } else if (method_type == HEAD && errors == 0) {
            head(connfd, size, resp);
        } else if (method_type == PUT && errors == 0) {
            put(filename, connfd, buf, header);
        }

        // reset buffers
        memset(&buf, 0, sizeof(buf));
        memset(&header, 0, sizeof(header));
        memset(&body, 0, sizeof(body));
        memset(&resp, 0, sizeof(resp));
        memset(&method, 0, sizeof(method));
        memset(&filepath, 0, sizeof(filepath));
        memset(&vers, 0, sizeof(vers));

        close(connfd);
    }
    return 0;
}
