V-Collide release 2.01
  Copyright (c) University of North Carolina

nbody and nbody2:
----------------

In this demo, we have 20 objects and we perform collision
detection over 100 frames. The paths followed by the objects
have been pre-computed and are read from an input file.

In the nbody demo, the program just reports which are the
intersecting objects, and in the nbody2 demo, the intersecting
triangles of each pair of intersecting objects are also
reported.


Format of the input files:
----

for polytope:
--
first, the number of triangles in the model

then, for each triangle, there are three points (each is a
vector of three numbers).


for transformations:
--
each line contains a number, intended to be used a double
precision floating point number.

each set of 16 successive lines represents a transformation
matrix in row-major order.

each set of 20 successive transformation matrices represents
the transformations for all 20 objects for a single frame.

there are 100 such frames in the simulation.


USAGE:
-----
nbody input/obj20.inp trans/trans

nbody2 input/obj20.inp trans/trans
