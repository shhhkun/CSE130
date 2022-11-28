EXECBIN  = httpserver

SOURCES  = $(wildcard *.c)
OBJECTS  = $(SOURCES:%.c=%.o) queue.o

CC       = clang
CFLAGS   = -Wall -Wpedantic -Werror -Wextra

.PHONY: all

all: $(EXECBIN)

$(EXECBIN): $(OBJECTS)
	$(CC) -o $@ $^ -pthread

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o
	rm -f httpserver

format:
	clang-format -i -style=file *.[ch]

