#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BLOCK 4096
#define STDOUT 1

int main (int argc, char  *argv[]) {
	uint8_t buf[BLOCK];
	int count; // # of bytes for read/write calls
	int i = 1;
	
	if (argc == 1) { // no file parameters given so use stdin
		printf("No files given\n");

		return 0;

	} // otherwise params given

	int infile = open(argv[i], O_RDONLY);

	uint8_t delim = (int) argv[argc - 1][0];
	
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
	return 0;
}
