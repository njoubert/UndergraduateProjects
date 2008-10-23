CFLAGS = -w -O $(GRAPHICS_CFLAGS)

VC_TOP     = ../..

INCLUDES  = -I$(VC_TOP)/include $(GRAPHICS_INCLUDES)


LDFLAGS   = -L$(VC_TOP)/lib -L$(VC_TOP)/RAPID $(GRAPHICS_LDFLAGS)

LIBS       = -lVCollide -lRAPID $(GRAPHICS_LIBS) -lm



OBJS   = polytope.o graphics.o main.o
TARGET = torii

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.C.o :
	$(CC) $(CFLAGS) $(INCLUDES) -c $<


clean: 
	rm -f $(OBJS) $(TARGET)


