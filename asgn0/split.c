#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <err.h>
#include <errno.h>

#define BLOCK  4096
#define STDIN  0
#define STDOUT 1

int main(int argc, char *argv[]) {
    int x = 0; // default return value of program
    uint8_t buf[BLOCK];
    int count; // # of bytes for read/write calls
    uint8_t delim = (int) argv[argc - 1][0];

    if (argc < 3) { // not enough args
        warnx("not enough arguments\n"
              "usage: %s <files> <delimiter>",
            argv[0]);
        exit(1);
    }

    if (strlen(argv[argc - 1]) > 1) { // delimiter is longer than 1 char
        warnx("only single-character delimiters allowed\n"
              "usage: %s <files> <delimiter>",
            argv[0]);
        exit(1);
    }

    for (int i = 1; i < argc - 1; i += 1) {
        if (strcmp(argv[i], "-") == 0) { // if file is '-' take input from stdin
            while ((count = read(STDIN, buf, BLOCK)) > 0) {
                for (int i = 0; i < count; i += 1) {
                    if (buf[i] == delim) {
                        buf[i] = '\n';
                    }
                }
                write(STDOUT, buf, count);
            }
        } else { // otherwise open specified file
            int infile = open(argv[i], O_RDWR);
            if (infile == -1) { // if filename doesnt exist, is a directory, or has no read perms
                warnx("%s: %s", argv[i], strerror(errno));
                x = 1;
            }

            while ((count = read(infile, buf, BLOCK)) > 0) {
                for (int i = 0; i < count; i += 1) {
                    if (buf[i] == delim) {
                        buf[i] = '\n';
                    }
                }
                write(STDOUT, buf, count);
            }
            close(infile);
        }
    }
    return x;
}
