CC=gcc
LD=gcc
CFLAGS=-g -Wall -std=c99
LDFLAGS=

OBJS=vector.o test.o
PROG=vector.test

$(PROG): $(OBJS)
	$(LD) -o $(PROG) $(OBJS) $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $<

test: $(PROG)
	@./$(PROG)

clean: 
	-rm -rf core *.o *~ "#"*"#" Makefile.bak $(PROG)
        
vector.c: vector.h
test.c: vector.h
