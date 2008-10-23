V-Collide release 2.01
  Copyright (c) University of North Carolina

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

format of input file:
---

the format of the input file is similar to the one used in
rings simulation (ringz.itri).

We would like to thank the Stanford Computer Graphics Laboratory
for the bunny models.




USAGE:
-----

bunny input/bunnyfile no-of-instances


