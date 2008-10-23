V-Collide release 2.01
  Copyright (c) University of North Carolina

Release 2.01 of V-Collide contains two collision detection
libraries (libVCollide, libRAPID) All of the code is written
in ANSI-compliant C and C++ and requires an ANSI C++
compiler to generate the libraries and executables.

Typing "make" in this top-level directory should start cause
all the object files, libraries, and executables to be
compiled. The Makefiles in the sub-directories are intended
to be used by the top-level Makefile, they may not
work if invoked directly.

If you use a compiler besides g++, replace the "CC = g++"
line in the Makefile in every subdirectory. Some of the
demos can also display the simulation graphically using
OpenGL. If you want to see the graphical simulations, you
will need to modify the first few lines of the top-level
Makefile to indicate the proper libraries, etc.

To write a C++ application using V-Collide, you must
#include <VCollide.H> and link with libVCollide.a,
libRAPID.a and libm.a (the standard math library).

See the manuals in doc/ and the examples in demos/. 


For users compiling using Microsoft Visual C++ 5.0
--------------------------------------------------
Go to MSVC_Compile directory, open the project workspace
VCollide2.dsw with Microsoft Developer Studio, and build
the demo projects. This will produce the V-Collide 2.01
library VCollide.lib in the directory lib\, and also
produce the executable demos in the directories demos\XXX,
where XXX is the name of each demo. 


Differences between V-Collide 1.0 and V-Collide 2.01
----------------------------------------------------
The main difference between V-Collide 1.0 and V-Collide 2.01 
is that V-Collide 2.01 can invoke RAPID to detect all the
intersections between each pair of intersecting objects, and
report all the intersecting triangles. 

Unlike V-Collide 1.0, V-Collide 2.01 no longer provides C 
interface. If your old programs are using the V-Collide 1.0
C++ interface, they need some modifications before being 
able to compile with V-Collide 2.01. Most of the changes
will be on the reporting of the collision results.

Please see the user manual in doc/ and the examples in demos/
to see how to make the changes.


Notes:
-----
RAPID release 2.01, by Stefan Gottschalk, is packaged with
this distribution, providing libRAPID.a.  Newer versions can
be obtained from the UNC collision web site:
http://www.cs.unc.edu/~geom/.

The quaternion library, developed originally by Ken Shoemake
and modified by Rich Holloway at UNC-Chapel Hill is also
included with this distribution, since the rings demo uses
it. It is not used by RAPID or V-Collide.

V-Collide mailing list:
----------------------
We would be interested in knowing more about your
application as well as any bugs you may encounter in the
collision detection library.  You can report them by sending
e-mail to geom@cs.unc.edu.


Amol Pattekar
Jon Cohen
Tom Hudson
Stefan Gottschalk
Ming Lin
Dinesh Manocha
