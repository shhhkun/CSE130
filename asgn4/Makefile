CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra
LDFLAGS = -lm

all: httpserver

bind.o: bind.c bind.h
	$(CC) $(CFLAGS) -c bind.c

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c

httpserver: httpserver.o bind.o queue.o
	$(CC) $(LDFLAGS) -o httpserver httpserver.o bind.o queue.o

httpserver.o: httpserver.c bind.h queue.h
	$(CC) $(CFLAGS) -c httpserver.c

clean:
	rm -f *.o
	rm -f httpserver

format:
	clang-format -i -style=file *.[ch]
