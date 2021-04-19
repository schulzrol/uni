CC = gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic -ggdb
LDFLAGS=-pthread -lrt

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

$(EXECNAME): $(OBJS)
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(EXECNAME)
	./$(EXECNAME) $(ARGS)

clean:
	rm $(EXECNAME) $(OBJS)