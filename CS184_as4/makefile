#Basic Stuff -----------------------------
CC          = g++ -g -Wall -fmessage-length=0
cc          = gcc -g -Wall -fmessage-length=0
#-----------------------------------------
#Optimization ----------------------------
OPT   = -O2
#-----------------------------------------
TARGETS = raytracer
OBJECTS = raytracer.o
#-----------------------------------------
#-----------------------------------------
raytracer: raytracer.o
	$(CC) raytracer.o $(LDOPTS) -o raytracer
	
raytracer.o: 
	$(CC) Debug.cpp Image.cpp Algebra.cpp Primitives.cpp Scene.cpp Film.cpp Sampler.cpp Parser.cpp raytracer.cpp -c $(CCOPTS)
	
default: $(TARGETS)
	rm -rf *.o

clean:
	/bin/rm -f *.o $(TARGETS)
	
all: clean default
