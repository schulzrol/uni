CFLAGS=-std=c99 -Wall -Wextra -pedantic -g
PROGRAM=hello
SRCS=hello.c
OBJS=$(SRCS:.c=.o)


all: $(PROGRAM)

$(PROGRAM): $(OBJS)

clean:
	rm $(PROGRAM) $(OBJS)
