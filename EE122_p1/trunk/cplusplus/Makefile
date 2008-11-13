CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		cplusplus.o HTTPRequest.o HTTPResponse.o

LIBS =

TARGET =	cplusplus.exe

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
