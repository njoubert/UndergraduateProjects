V-Collide release 2.01
  Copyright (c) University of North Carolina

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


USAGE:
-----

The directory input/ contains several sub-directories, each of which
contains input files for different simulations.

The .psoup files give the geometry of the objects and the .path files
give the transformation matrices for the object's path.

This demo can be run as:

torii <psoup1> <path1> <psoup2> <path2>

For eg:

torii input/hooked1/model1.psoup input/hooked1/model1.path input/hooked1/model2.psoup input/hooked1/model2.path

