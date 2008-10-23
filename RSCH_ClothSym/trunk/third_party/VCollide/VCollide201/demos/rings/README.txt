V-Collide release 2.01
  Copyright (c) University of North Carolina

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


USAGE:
-----
rings ten-rings.movie

The command must be executed from the same directory in which the
files ten-rings.movie and ringz.itri are located.
