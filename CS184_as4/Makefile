CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		raytracer.o Image.o

LIBS = 	

TARGET =	raytracer

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
