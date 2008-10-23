VC_TOP     = ../..

CFLAGS = -w -O $(GRAPHICS_CFLAGS)

OBJS   = polytope.o graphics.o main.o

INCLUDES  = -I$(VC_TOP)/include -I./quatlib $(GRAPHICS_INCLUDES)

LDFLAGS   = -L$(VC_TOP)/lib -L$(VC_TOP)/RAPID -L./quatlib $(GRAPHICS_LDFLAGS)

LIBS       = -lVCollide -lRAPID $(GRAPHICS_LIBS) -lquat -lm

TARGET = rings

$(TARGET) : $(OBJS)
	cd quatlib; $(MAKE) CC=cc
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.C.o :
	$(CC) $(CFLAGS) $(INCLUDES) -c $<


clean: 
	cd quatlib; $(MAKE) clean
	rm -f $(OBJS) $(TARGET)

