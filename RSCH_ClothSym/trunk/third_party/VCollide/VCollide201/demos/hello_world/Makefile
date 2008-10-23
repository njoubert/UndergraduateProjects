VC_TOP     = ../..

CFLAGS = -w -O

LDFLAGS   = -L$(VC_TOP)/lib -L$(VC_TOP)/RAPID

INCLUDES  = -I$(VC_TOP)/include

all : simple simple2

simple : simple.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o simple simple.cpp -lVCollide -lRAPID -lm

simple2 : simple2.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o simple2 simple2.cpp -lVCollide -lRAPID -lm

clean: 
	rm -f simple simple2


