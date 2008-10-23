VC_TOP     = ../..

CFLAGS = -w -O

LDFLAGS   = -L$(VC_TOP)/lib -L$(VC_TOP)/RAPID

INCLUDES  = -I$(VC_TOP)/include

all : nbody nbody2

nbody : nbody.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o nbody nbody.cpp -lVCollide -lRAPID -lm

nbody2 : nbody2.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o nbody2 nbody2.cpp -lVCollide -lRAPID -lm

clean: 
	rm -f nbody nbody2


