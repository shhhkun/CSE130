CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra

all: queue.o

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c

clean:
	rm -f *.o

format:
	clang-format -i -style=file *.[ch]
