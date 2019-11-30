CC = gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -g
LDFLAGS=-lm

EXECNAME=proj2
SRCS=proj2.c
HEADERS=

# 'make' or 'make all' compiles executable
# 'make clean' - removes files produced mid compiling
# 'make ARGS="arg1 arg2 arg3" run' - recompiles & runs program

#====NO TOUCHIE TOUCHIE BELOW THIS LINE====#
.PHONY: all clean run

OBJS=$(SRCS:.c=.o)
	
all: $(EXECNAME)

$(EXECNAME): $(OBJS) $(LDFLAGS)
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(EXECNAME)
	./$(EXECNAME) $(ARGS)

clean:
	rm $(EXECNAME) $(OBJS)
