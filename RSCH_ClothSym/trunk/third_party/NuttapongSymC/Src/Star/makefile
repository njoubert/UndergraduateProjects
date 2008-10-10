# makefile for Star
#
# Type "make" to compile Star.
#
# Type "make distclean" to delete all executable files.

# SRC is the directory in which the C source files are, and BIN is the
#   directory where you want to put the executable programs.  By default,
#   both are the current directory.

SRC = ./
BIN = ./

# CC should be set to the name of your favorite C compiler.

CC = cc

# CSWITCHES is a list of all switches passed to the C compiler.  I strongly
#   recommend using the best level of optimization.  I also strongly
#   recommend timing each level of optimization to see which is the
#   best.  For instance, when I had a DEC Alpha using DEC's optimizing
#   compiler, the -O2 switch generated a notably faster version of Triangle
#   than the -O3 switch.  Go figure.
#
# By default, Star uses double precision floating point
#   numbers.  If you prefer single precision, use the -DSINGLE switch.
#   Double precision uses more memory, but improves the resolution of
#   the meshes you can generate with Star.  It also reduces the
#   likelihood of a floating exception due to overflow.  Also, it is
#   much faster than single precision on many architectures.  I recommend
#   double precision unless you want to generate a mesh for which you do
#   not have enough memory to use double precision.
#
# If yours is not a Unix system, use the -DNO_TIMER switch to eliminate the
#   Unix-specific timer code.
#
# To get the exact arithmetic to work right on an Intel processor, use the
#   -DCPU86 switch with Microsoft C, or the -DLINUX switch with gcc running
#   on Linux.  The floating-point arithmetic might not be robust otherwise.
#   Please see http://www.cs.cmu.edu/~quake/robust.pc.html for details.
#
# If you are modifying Star, I recommend using the -DSELF_CHECK switch
#   while you are debugging.  Defining the SELF_CHECK symbol causes
#   Star to include self-checking code.  Star will execute more
#   slowly, however, so be sure to remove this switch before compiling a
#   production version.
#
# An example CSWITCHES line is:
#
#   CSWITCHES = -O -DNO_TIMER -DLINUX -I/usr/local/include -L/usr/local/lib

CSWITCHES = -g -DSELF_CHECK -Wall -Wconversion -Wstrict-prototypes -I/usr/X11R6/include -L/usr/X11R6/lib

# STARLIBDEFS is a list of definitions used to compile an object code version
#   of Star (Star.o) to be called by another program.  The file
#   "Star.h" contains detailed information on how to call Star.o.
#
# The -DSTARLIBRARY should always be used when compiling Star into an
#   object file.
#
# An example STARLIBDEFS line is:
#
#   STARLIBDEFS = -DSTARLIBRARY -DREDUCED -DCDT_ONLY

STARLIBDEFS = -DSTARLIBRARY

# RM should be set to the name of your favorite rm (file deletion program).

RM = /bin/rm

# The action starts here.

all: $(BIN)Star $(BIN)example

starlibrary: $(BIN)Star.o $(BIN)starcall

$(BIN)starcall: $(BIN)starcall.c $(BIN)Star.o
	$(CC) $(CSWITCHES) -o $(BIN)starcall $(SRC)starcall.c \
		$(BIN)Star.o -lm

$(BIN)Star.o: $(SRC)Star.c $(SRC)Star.h
	$(CC) $(CSWITCHES) $(STARLIBDEFS) -c -o $(BIN)Star.o \
		$(SRC)Star.c

$(BIN)Star: $(SRC)Star.c
	$(CC) $(CSWITCHES) -o $(BIN)Star $(SRC)Star.c -lm

$(SRC)Star.c: $(SRC)startop.c $(SRC)interact.c $(SRC)misc.c $(SRC)arraypool.c $(SRC)proxipool.c $(SRC)zorder.c $(SRC)allocmap.c $(SRC)linkring.c $(SRC)link2d.c $(SRC)primitives.c $(SRC)tetcomplex.c $(SRC)input.c $(SRC)output.c $(SRC)bowyerwatson.c $(SRC)locate.c $(SRC)incremental.c $(SRC)constrain.c $(SRC)statistics.c $(SRC)starmain.c
	cat $(SRC)startop.c $(SRC)interact.c $(SRC)misc.c $(SRC)arraypool.c $(SRC)proxipool.c $(SRC)zorder.c $(SRC)allocmap.c $(SRC)linkring.c $(SRC)link2d.c $(SRC)primitives.c $(SRC)tetcomplex.c $(SRC)input.c $(SRC)output.c $(SRC)bowyerwatson.c $(SRC)locate.c $(SRC)incremental.c $(SRC)constrain.c $(SRC)statistics.c $(SRC)starmain.c > $(SRC)Star.c

$(BIN)example: $(SRC)example.c
	$(CC) $(CSWITCHES) -o $(BIN)example $(SRC)example.c -lm

$(SRC)example.c: $(SRC)startop.c $(SRC)interact.c $(SRC)misc.c $(SRC)arraypool.c $(SRC)proxipool.c $(SRC)zorder.c $(SRC)allocmap.c $(SRC)linkring.c $(SRC)link2d.c $(SRC)primitives.c $(SRC)tetcomplex.c $(SRC)input.c $(SRC)output.c $(SRC)bowyerwatson.c $(SRC)locate.c $(SRC)incremental.c $(SRC)constrain.c $(SRC)statistics.c $(SRC)examplemain.c
	cat $(SRC)startop.c $(SRC)interact.c $(SRC)misc.c $(SRC)arraypool.c $(SRC)proxipool.c $(SRC)zorder.c $(SRC)allocmap.c $(SRC)linkring.c $(SRC)link2d.c $(SRC)primitives.c $(SRC)tetcomplex.c $(SRC)input.c $(SRC)output.c $(SRC)bowyerwatson.c $(SRC)locate.c $(SRC)incremental.c $(SRC)constrain.c $(SRC)statistics.c $(SRC)examplemain.c > $(SRC)example.c

distclean:
	$(RM) $(BIN)Star $(BIN)Star.o $(BIN)starcall $(BIN)example
