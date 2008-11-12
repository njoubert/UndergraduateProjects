# Makefile for CS61c Proj1

# Add additional source files here. Each file should be seperated
# by a space. For example:
#
#   SRCFILES := i_memory.c core.c microscheme.c
#
SRCFILES := i_memory.c core.c microscheme.c vector.c bignum.c
TESTSUITE:= unittest.c
# If you used additional C libraries, add them here. Each libary
# should be seperated by a space. For example:
#
#  ADDL_LIB := -lm -lcrypt -lzlib
#
ADDL_LIB := -lm

########################################################################
#  You shouldn't need to modify below this line
########################################################################
EXEC := microscheme
TEST_NUM_EXEC := bignum_test
TEST_VEC_EXEC := vector_test

##Automatic compilation of .c to .o files

OBJS := $(SRCFILES:.c=.o)
TESTER := $(TESTSUITE:.c=.o)
CC := gcc
CFLAGS := -g -Wall -std=c99 
LDFLAGS := -g -Wall -std=c99 $(ADDL_LIB)
ifneq (,$(findstring CYGWIN,$(shell uname)))
	CFLAGS += -DCYGWIN
	LDFLAGS += -DCYGWIN
	EXEC := $(EXEC).exe
endif

proj1 : $(OBJS)
	$(CC) -Wall -g -o $(EXEC) $(OBJS)
	
### TEST CASES START HERE ###

test_num: $(OBJS)
	$(CC) $(CFLAGS) -o tests/$(TEST_NUM_EXEC) $(TESTER) bignum.o test_num.c vector.o 
	@./tests/$(TEST_NUM_EXEC)
	
test_vector: $(OBJS) 
	$(CC) $(CFLAGS) -o tests/$(TEST_VEC_EXEC) $(TESTER) test_vector.c vector.o
	@./tests/$(TEST_VEC_EXEC)

test: $(TESTER) test_num test_vector #Just depends on all the test cases
	
	
### TEST CASES END HERE ###

clean :
	\rm -rf *~ *.o $(EXEC)
