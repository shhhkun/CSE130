#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BLOCK  4096
#define STDIN 0
#define STDOUT 1

int main(int argc, char *argv[]) {
    uint8_t buf[BLOCK];
    int count; // # of bytes for read/write calls
    uint8_t delim = (int) argv[argc - 1][0];

    if (strlen(argv[argc - 1]) > 1) { // delimeter is longer than 1 char
        printf("Delimiter invalid\n");
        return 0;
    }

    if (argc == 1) { // no file parameters given
        printf("No files given\n");
        return 0;
    } // otherwise params given

    for (int i = 1; i < argc - 1; i += 1) {
        if (argv[i][0] == '-' && strlen(argv[i]) == 1) { // if file is '-' take input from stdin
		while ((count = read(STDIN, buf, BLOCK)) > 0) {
			for (int i = 0; i < count; i += 1) {
				if (buf[i] == delim) {
					buf[i] = '\n';
				}
			}
			write(STDOUT, buf, count);
		}
        } else {
		int infile = open(argv[i], O_RDONLY);

		if (infile == -1) {
			printf("Failed to open file\n");
			return 0;
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
