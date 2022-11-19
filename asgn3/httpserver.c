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

#define OPTIONS "t:l:"

int isvalid(char *str, int type) {
    int i = 0;
    if (type == 0) { // check [a-z][A-Z]
        if (strlen(str) > 8 || str[0] == '\0') {
            return 1;
        }
        while (str[i] != '\0') {
            if (isalpha(str[i]) == 0) {
                return 1;
            }
            i += 1;
        }
    } else if (type == 1) { // check [a-z][A-Z][0-9]_.
        if (strlen(str) > 19 || str[0] == '\0') {
            return 1;
        }
        while (str[i] != '\0') {
            if (isalnum(str[i]) == 0 && str[i] != '.' && str[i] != '_') {
                return 1;
            }
            i += 1;
        }
    }
    return 0;
}

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
    case GET: return "GET";
    case HEAD: return "HEAD";
    case PUT: return "PUT";
    case 200: return "OK";
    case 201: return "Created";
    //case 400: return "Bad Request";
    //case 403: return "Forbidden";
    case 404: return "Not Found";
    case 500:
        return "Internal Server Error";
        //case 501: return "Not Implemented";
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
    return;
}

// <method/oper>,<URI>,<status code>,<request ID>\n
void logentry(int fd, char *logresp, int method, char *file, int code, int reqid) {
    sprintf(logresp, "%s,/%s,%d,%d\n", phrase(method), file, code, reqid);
    write(fd, logresp, strlen(logresp));
    return;
}

void get(
    char *file, int connfd, char *buf, int size, char *resp, char *logresp, int logfd, int reqid) {
    int count;
    int fd = open(file, O_RDONLY);
    response(connfd, resp, NULL, 200, size); // 200 'OK'
    while ((count = read(fd, buf, 4096)) > 0) {
        write(connfd, buf, count);
    }
    close(fd);
    logentry(logfd, logresp, GET, basename(file), 200, reqid);
    return;
}

void head(char *file, int connfd, int size, char *resp, char *logresp, int logfd, int reqid) {
    response(connfd, resp, NULL, 200, size); // 200 'OK'
    logentry(logfd, logresp, HEAD, basename(file), 200, reqid);
    return;
}

void put(char *file, int connfd, char *buf, char *header, char *logresp, int logfd, int reqid) {
    int content_len;
    int fd, count, bytes_r = 0;
    char *ptr = strstr(header, "Content-Length:"); // move ptr to content len
    int code = 200; // default 200 'OK'
    if (ptr != NULL) {
        if (access(file, F_OK) == -1) { // if file doesn't exist
            fd = open(file, O_CREAT | O_WRONLY, 0600); // -rw --- ---
            code = 201; // change response to 201 'Created'
        } else { // if file does exist
            fd = open(file, O_TRUNC | O_WRONLY);
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
            logentry(logfd, logresp, PUT, basename(file), 200, reqid);
        } else {
            response(connfd, buf, "Created\n", code, 8); // 201 'Created'
            logentry(logfd, logresp, PUT, basename(file), 201, reqid);
        }
    }
    return;
}

int main(int argc, char *argv[]) {
    char buf[4096];
    char resp[4096];
    char logresp[2048];
    char header[2048];
    char hfield[2048];
    int port, reqid, errors = 0, not_rq = 1, malformed_header = 0; // 0 'false', 1 'true'
    char method[2048], filepath[2048], vers[2048];
    char key[500], value[500];
    struct stat st;
    int numthreads = 4; // default 4 threads
    int logfd = 2; // default stderr

    memset(&buf, 0, sizeof(buf));
    memset(&hfield, 0, sizeof(hfield));
    memset(&header, 0, sizeof(header));

    if (argc < 2) {
        warnx("wrong arguments: %s port_num\n"
              "usage: %s [-t threads] [-l logfile] <port>",
            argv[0], argv[0]);
        exit(1);
    }

    int opt = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 't': {
            numthreads = atoi(optarg);
        }

        case 'l': {
            if (access(optarg, F_OK) == -1) {
                logfd = open(optarg, O_CREAT | O_WRONLY, 0600);
            } else {
                logfd = open(optarg, O_TRUNC | O_WRONLY);
            }
        }
        }
    }

    if (validport(argv[argc - 1]) == 1) { // if port invalid
        warnx("invalid port number: %s", argv[1]);
        exit(1);
    } else {
        port = strtol(argv[argc - 1], NULL, 10); // take port number
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
        char *filename = filepath + 1; // remove '/' from /path
        int method_type = methodtype(method);
        int validm = isvalid(method, 0);
        int validf = isvalid(basename(filename), 1);

        stat(filename, &st);
        int size = st.st_size; // get file size (in bytes)

        char *ptr = strstr(header, "Request-Id:"); // move ptr to req id
        if (ptr == NULL) {
            reqid = 0;
        } else {
            sscanf(ptr, "Request-Id: %d", &reqid);
        }

        if ((strcmp(vers, "HTTP/1.1") != 0 || vers[0] == '\0' || malformed_header == 1
                || validm == 1 || validf == 1)
            && errors == 0) {
            response(connfd, buf, "Internal Server Error\n", 500,
                22); // if wrong HTTP vers, missing stuff, malformed h-field, or wrong range/len
            errors = 1;
            malformed_header = 0;
        }
        if ((access(filename, F_OK) == -1 && errors == 0) && method_type != PUT) {
            response(
                connfd, buf, "Not Found\n", 404, 10); // if file doesn't exist and not PUT request
            logentry(logfd, logresp, method_type, basename(filename), 404, reqid);
            errors = 1;
        }
        if (access(filename, F_OK) == 0) { // first check that file exists
            if ((access(filename, R_OK) == -1 && errors == 0) && method_type != PUT) {
                response(connfd, buf, "Internal Server Error\n", 500,
                    22); // if no read perms and not PUT request
                logentry(logfd, logresp, method_type, basename(filename), 500, reqid);
                errors = 1;
            }
            if ((access(filename, W_OK) == -1 && errors == 0) && method_type == PUT) {
                response(connfd, buf, "Internal Server Error\n", 500,
                    22); // if no write perms and is PUT request
                logentry(logfd, logresp, method_type, basename(filename), 500, reqid);
                errors = 1;
            }
        }
        if (S_ISDIR(st.st_mode) != 0 && errors == 0) {
            response(
                connfd, buf, "Internal Server Error\n", 500, 22); // if filepath leads to directory
            logentry(logfd, logresp, method_type, basename(filename), 500, reqid);
            errors = 1;
        }
        if (method_type != GET && method_type != HEAD && method_type != PUT && errors == 0) {
            response(connfd, buf, "Internal Server Error\n", 500, 22); // if invalid method
            errors = 1;
        }

        // handle request method
        if (method_type == GET && errors == 0) {
            get(filename, connfd, buf, size, resp, logresp, logfd, reqid);
        } else if (method_type == HEAD && errors == 0) {
            head(filename, connfd, size, resp, logresp, logfd, reqid);
        } else if (method_type == PUT && errors == 0) {
            put(filename, connfd, buf, header, logresp, logfd, reqid);
        }

        // reset buffers
        memset(&buf, 0, sizeof(buf));
        memset(&header, 0, sizeof(header));
        memset(&resp, 0, sizeof(resp));
        memset(&logresp, 0, sizeof(logresp));
        memset(&method, 0, sizeof(method));
        memset(&filepath, 0, sizeof(filepath));
        memset(&vers, 0, sizeof(vers));

        close(connfd);
    }
    return 0;
}
