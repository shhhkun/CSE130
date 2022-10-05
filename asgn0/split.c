#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <err.h>
#include <sys/stat.h>

#define BLOCK  4096
#define STDIN  0
#define STDOUT 1

int main(int argc, char *argv[]) {
    struct stat fpath;
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
            stat(argv[i], &fpath);
            if (S_ISREG(fpath.st_mode) == 0) { // check if filename is directory
                warnx("%s: Is a directory", argv[i]);
                exit(1);
            }

            int infile = open(argv[i], O_RDONLY);
            if (infile == -1) { // if file doesnt open
                warn("%s", argv[i]);
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
    return 0;
}
