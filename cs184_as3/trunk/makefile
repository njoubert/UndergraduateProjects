CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		shader.o

LIBS =

TARGET =	shader

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
