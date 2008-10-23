V-Collide release 2.01
  Copyright (c) University of North Carolina

Demos:
-----

This directory contains 5 demos:

hello_world -- simple amd simple2:
---------------------------------

This is a very simple demo, intended to demonstrate the
basic functionality of V-Collide. The demo consists of two
unit cubes, each moving along the X axis towards the other
and finally colliding and crossing each other. 

In the simple demo, the program just reports which are the
intersecting objects, and in the simple2 demo, the intersecting
triangles of each pair of intersecting objects are also
reported.


nbody and nbody2:
----------------

The nbody demo in only slightly more involved than the
hello_world demos. In this demo, we have 20 objects and we
perform collision detection over 100 frames. The paths
followed by the objects have been precomputed and are read
from an input file.

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


torii:
-----

This demo is a simulation of two wrinkled torii, moving and
at times colliding with each other. As in the nbody demo,
the motion sequences have been precomputed and are read from
input files. This demo can also be compiled with OpenGL
display of the simulation enabled. For instructions, see the
top-level README file.

Format of input files:
---

for .path files:
--
first, a number which is the number of time steps in the path.
Each time step is a rotation matrix and a translation vector.

for .psoup files:
--
first, the number of triangles in the model

then, for each triangle, there are three points (each is a
vector of three numbers), and a normal for the triangle
face.


rings:
-----

This demo consists of a simulation of 10 rings. As in
earlier demos, we use pre-computed motion sequences read
from an input file. This demo can also be compiled with
OpenGL display of the simulation.

We would like to thank Jim Cremer at the University
of Iowa for the rings simulation.

Format of input files:
---

for ten-rings.movie:
--
first line just indicates the number of bodies, 10.

the next ten lines describe the bodies in the simulation:
name geometry-file-name color-index

the only the only thing we care about is the
geometry-file-name, ringz.itri.  Here, the shapes of all 10
rings are the same.

after the initial 11 lines come the simulation frames.  Each
frame starts with a line indicating the time (e.g. "t 0" "t
5.57") and is followed by 10 lines giving the position and
orientation of each ring. Pos and orientation is given as 
 x y z q0 q1 q2 q3
where x, y, z is mass center position and q0,q1,q2,q3 is
Euler parameter/quaternion rep of orientation (e.g. 1,0,0,0
is orientation aligned with global frame, while
.707..., 0, 0, .707.... is rotation 90 degrees about Z axis)

there are 558 frames.

for ringz.itri:
--
We don't need all the data in the file.
The first two numbers of line 2 are the number of vertices
and the number of triangles respectively. After line 2 are
the vertices, one on each line and indexed from 0. After the
vertices is a list of triangles, one on each line and the
numbers index into the list of vertices.


bunny:
-----

This is a relatively more involved demo. In this demo, there
are a number of bunnies inside a cube, each having some
random linear and angular velocity. These bunnies move along
paths determined by their velocities. During the simulation,
the bunnies might collide with each other or with the walls
of the cube. A very simple collision response is also
modeled. (It should be noted that this collision response is
a very simple one and might not be physically accurate). One
of the walls of the cube is designated as a "sink" and if a
bunny collides with this wall, then if is removed from the
simulation. But at the same time, another bunny is added at
the opposite wall, so that the number of bunnies remains
constant. This demo can also be compiled with OpenGL display
of the simulation.


We would like to thank the Stanford Computer Graphics Laboratory
for the bunny model.

format of input file:
---

the format of the input file is similar to the one used in
rings simulation (ringz.itri).


We would like to thank the Stanford Computer Graphics Laboratory
for the bunny models.




--
A. Pattekar
T. Hudson
J. Cohen
S. Gottschalk
M. Lin
D. Manocha

