#####################################################
# INITIAL SETUP
#####################################################

CC := gcc
CFLAGS := -O2 -g -Wall -fmessage-length=0
LIBS := 
SOLARIS_LIBS :=  -lnsl -lsocket -lresolv

#####################################################
# FILES IN BUILD
#####################################################

COMMON_OBJS = 		common.o httpsyntax.o
SERVER_OBJS =		server.o
CLIENT_OBJS = 		client.o
TEST_OBJS   =		test.o

SERVER_TARGET = http_server
CLIENT_TARGET = http_client
TEST_TARGET = test.out

#####################################################
# BUILD ACTIONS
#####################################################

$(CLIENT_TARGET):	$(COMMON_OBJS) $(CLIENT_OBJS) 
	$(CC) $(CFLAGS) $(LFLAGS) -o $(CLIENT_TARGET) $(CLIENT_OBJS) $(COMMON_OBJS) $(LIBS)

$(SERVER_TARGET):	$(COMMON_OBJS) $(SERVER_OBJS) 
	$(CC) $(CFLAGS) $(LFLAGS) -o $(SERVER_TARGET) $(SERVER_OBJS) $(COMMON_OBJS) $(LIBS)
	
$(TEST_TARGET):		$(COMMON_OBJS) $(TEST_OBJS)
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TEST_TARGET) $(TEST_OBJS) $(COMMON_OBJS) $(LIBS)

#####################################################
# ACTIONS FOR DEALING WITH PLATFORM DIFFERENCES
#####################################################	



#####################################################
# PUBLIC BUILD CALLS
#####################################################

testit: clean $(TEST_TARGET)
	valgrind -v --leak-check=full ./$(TEST_TARGET)

all: clean http_client http_server

clean:
	rm -f $(SERVER_OBJS) $(COMMON_OBJS) $(CLIENT_OBJS) $(TEST_OBJS) $(SERVER_TARGET) $(CLIENT_TARGET) $(TEST_TARGET)

	