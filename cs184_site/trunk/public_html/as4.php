<td class="main_content_box">

<table width="100%" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td class="left_content">

<i>Negative Space Follows...</i>

	</td>
        <td class="body_content">

<h2>Assignment 4</h2>

This assignment builds a fully functional ray tracer. Pretty sweet? yea, i know.
<br>Go to the <a href="#bottom">bottom of the doc</a> to see what I turned in.

<p>
<h3>First Steps - Writing images!</h3>
<br>Since we need to output pretty things, we need o be able to write pixels to disk somehow. I'm going to go with writing BMP files, since the format is very straightforward. I opted to roll my own code to do this, rather than mess with libraries. This keeps my executable size small, makes linking easy, teaches me more about image format, and frankly only takes about 20 lines. I coded up the basic procedure to write out the headers and pixel data, which i'm storing internally in a dynamically allocated array. Here is my first Blank Screen of Death:
<br><center><img src="images/rt-BSOD.jpg"/> </center>
<br>Yes, it is a little blank box. Wee!
<br>This is a crucial piece of code to get right, so i generated a couple of test images to make sure it works nicely. I iterated over all the pixels in the image, alternatively moving from 0 to 255 color values along the x and y values, alternating between red, green and blue. The output was exactly what I expected it to be, so at least this part works!
<br><img src="images/rt-redgreen.jpg"/>&nbsp<img src="images/rt-bluegreen.jpg"/>

<p>
	<h3>File IO, Scene and Film classes, Debugging framework</h3>
I decided to implement many of the less "glamorous" features before moving on to outputting fancy graphics. I designed my raytracer as a whole (documented in the README file I will post further down) and implemented .OBJ file parsing (which I slightly extended to support my definition of spheres and lights), argument parsing, Scene generation (which consists of an eye, viewport, lights and primitives), Sampling the viewport and aggregating these samples into a film, which sets output to my image library.
This took quite a while, as you can imagine, and I wrote unit tests and significant debugging hooks. I implemented 5 levels of debugging printout, as well as error and info output, which logs a timestamp, function, file and line number. I tested as I implemented to ensure my code works as I expect it to. Finally I could parse full OBJ files (see the teapot.obj posted under examples), create all the necessary memory structures, and set it up ready for raytracing. 

<h3>Hitting spheres</h3>
So far, I had no cool output to show yet, thus I started working on ray intersection code around 2am today. It didn't take long to get basic sphere intersection going, and here's the fruits of my labors. As you can see, I can already instantiate multiple copies of a sphere, and move my camera perspective around in all kinds of cool ways:
<br><img src="images/rt-1firstsphere.jpg" width=400 height=200/>
<br><img src="images/rt-2multispheres.jpg" width=400 height=400/>
<br><img src="images/rt-3cameraperspective.jpg" width=400 height=400/>
<br>
<br>After getting a good framework in place, I was ready to hammer out the core part of the raytracer itself - intersection testing and ray casting. My ray casting procedure itself was fairly easy, since I could recycle all my shading code from the previous assignment, and I already had routines to calculate vector reflections and sphere normals. I used this to create shadow rays by casting a ray to each light, and have that affect the shading calculation. Reflections is done by calculating a reflection vector around the normal of the surface, and recursively calling my raytracing procedure on this new ray.

<h3>Shading!</h3>
Ambient Shading was the first to be implemented and thoroughly tested, since this proved that my raytracing and intersection tests were working just fine:
<br><img src="images/rt-4ambientcolors.jpg" width=400 height=400/>
<br>I realized that I had a bug in finding the intersection point that did not guarantee the correct order along the ray, so after a slight modification I could draw overlapping spheres:
<br><img src="images/rt-5overlapping.jpg" width=400 height=400/>
<br>I made the necessary modifications for my shading code from the previous assignment to work with the rays I have available:
<br><img src="images/rt-6shading.jpg" width=400 height=400/>

<h3>Reflections and Shadows! Easy as Pi!</h3>
Two more modifications - one to cast rays to the light and check for an occluder, and one to recursively shade reflection rays. I'm extremely excited at this point:
<br><img src="images/rt-7reflectionsshadows.jpg" width=400 height=400/>
<br>I wrote a little ruby script to generate environments for me, and here's a nice image with 6x6x3 cube of spheres, all reflecting off each other (click to enlarge):
<br><a href="images/rt-8many.jpg"><img src="images/rt-8many.jpg" width=400 height=400/></a>

<h3>Triangles</h3>
I also managed to get my first triangle intersection code working. The normals aren't calculated correctly yet, thus you only see the ambient color term, but the intersection tests work!
<br><img src="images/rt-9triangle.jpg" width=400 height=400/>
<br>I started having a lot of strange images when I tried doing more than 1 reflection, and after a long night of struggling my buddy <a href="http://inst.cs.berkeley.edu/~cs184-ak/">Navin</a> finally pointed out that my rays are intersecting with the object they are reflecting off, since I didn't add a reflection bias to the ray. This ended up being an easy fix, and I started doing some pretty cool reflections. Here's some more eye candy:
<br><img src="images/rt-10colorcube.jpg" width=400 height=400/>
<br>Let's add some reflections! It gets crazy since I'm clipping colors:
<br><img src="images/rt-11colorcubewithref.jpg" width=400 height=400/>
<br>My triangle code doesn't seem to play too nicely yet... what's going on?!
<br><img src="images/rt-12shadows.jpg" width=400 height=400/>
<br>But my spheres make pretty reflections!
<br><img src="images/rt-13spherereflections.jpg" width=400 height=400/>
<br>I've been having a lot of issues with triangles, and finally found that I was not calculating the ray parameter correctly. After fixing the numerical issues, I could finally draw some pretty pictures!
<br><img src="images/rt-15reflectionswork.jpg" width=400 height=400/>

<h3>Fun with elaborate scenes</h3>
<img src="images/rt-16firstscene.jpg" width=400 height=400/>
<br>And I could finally render that damn teapot!!!!! Notice how I only do flat shading at the moment. I'll attempt to interpolate normals soon enough...
<br><img src="images/rt-17teapot1.jpg" width=400 height=400/>
<br>Things are looking SWEEEET!!!!! Click for a 1600x1600 rendering. This scene includes a pointlight shining in from the left and a directional light from up top. I use multiple spheres, triangles, 5 levels of reflection and a variety of color and shading settings.
<br><a href="images/rt-19CoolSceneHR.png"><img src="images/rt-19CoolScene.png" width=400 height=400/></a>

<h3>Adding Transforms - For Ellipsoids!</h3>
It was actually very simple to implement ellipsoids from here. Ellipsoids are just unit spheres with a scale and/or rotation applied to them. I opted that the user supply us with scale and rotation parameters, and I built two matrices for each ellipsoid to store these two values. You can multiply them together, but its actually easier and faster to apply them if you don't use all that linear algebra routines you wrote and just keep them separate! For the intersection test, you inversely transform the given ray to enter the ellipsoid's object space. This allows us to test for an intersection using the good old sphere intersection code I wrote, what, 4 days ago? To calculate the normals you simply transform the given intersection point from world to object coordinates and run the sphere's normal calculation code once more! Pretty neat'o. Here's the same scene as above, but with a highly reflective ellipsoid now in the center:
<br><img src="images/rt-20withellipsoids.jpg" width=400 height=400/>
</p>
</p>

<h3><a name="bottom">What I turned in</a></h3>
Since a cool site isn't all we get for grades, here's my submission package:

<table border="0" cellspacing="5" cellpadding="5">

	<tr>
		<th><a href="as4/image-01.jpg"><img src="as4/image-01s.jpg"/></a></th>
		<th><a href="as4/input-01.scene">Input 01</a></th>
		<th>590ms</th>
		<th><a href="as4/notes-01">First Sphere Intersections</a></th>
	</tr>
	
	<tr>
		<th><a href="as4/image-02.jpg"><img src="as4/image-02s.jpg"/></a></th>
		<th><a href="as4/input-02.scene">Input 02</a></th>
		<th>900ms</th>
		<th><a href="as4/notes-02">Multiple spheres, ambient colors</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-03.jpg"><img src="as4/image-03s.jpg"/></a></th>
		<th><a href="as4/input-03.scene">Input 03</a></th>
		<th>810ms</th>
		<th><a href="as4/notes-03">Camera perspective</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-04.jpg"><img src="as4/image-04s.jpg"/></a></th>
		<th><a href="as4/input-04.scene">Input 04</a></th>
		<th>820ms</th>
		<th><a href="as4/notes-04">Z-ordering</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-05.jpg"><img src="as4/image-05s.jpg"/></a></th>
		<th><a href="as4/input-05.scene">Input 05</a></th>
		<th>1110ms</th>
		<th><a href="as4/notes-05">Phong Shading</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-06.jpg"><img src="as4/image-06s.jpg"/></a></th>
		<th><a href="as4/input-06.scene">Input 06</a></th>
		<th>2510ms</th>
		<th><a href="as4/notes-06">Basic reflections</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-07.jpg"><img src="as4/image-07s.jpg"/></a></th>
		<th><a href="as4/input-07.scene">Input 07</a></th>
		<th>2090ms</th>
		<th><a href="as4/notes-07">Multiple reflections</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-08.jpg"><img src="as4/image-08s.jpg"/></a></th>
		<th><a href="as4/input-08.scene">Input 08</a></th>
		<th>1830ms</th>
		<th><a href="as4/notes-08">Reflections with Shadows</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-09.jpg"><img src="as4/image-09s.jpg"/></a></th>
		<th><a href="as4/input-09.scene">Input 09</a></th>
		<th>570ms</th>
		<th><a href="as4/notes-09">First Triangle!</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-10.jpg"><img src="as4/image-10s.jpg"/></a></th>
		<th><a href="as4/input-10.scene">Input 10</a></th>
		<th>1310ms</th>
		<th><a href="as4/notes-10">First Directional Light</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-11.jpg"><img src="as4/image-11s.jpg"/></a></th>
		<th><a href="as4/input-11.scene">Input 11</a></th>
		<th>13800ms?</th>
		<th><a href="as4/notes-11">Stress-test Triangle Code</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-12.jpg"><img src="as4/image-12s.jpg"/></a></th>
		<th><a href="as4/input-12.scene">Input 12</a></th>
		<th>1960ms</th>
		<th><a href="as4/notes-12">Triangle Reflections!</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-13.jpg"><img src="as4/image-13s.jpg"/></a></th>
		<th><a href="as4/input-13.scene">Input 13</a></th>
		<th>2210ms</th>
		<th><a href="as4/notes-13">Input 7 with Directional Lights!</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-14.jpg"><img src="as4/image-14s.jpg"/></a></th>
		<th><a href="as4/input-14.scene">Input 14</a></th>
		<th>2340ms</th>
		<th><a href="as4/notes-14">Like Previous!</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-15.jpg"><img src="as4/image-15s.jpg"/></a></th>
		<th><a href="as4/input-15.scene">Input 15</a></th>
		<th>11940ms</th>
		<th><a href="as4/notes-15">First full scene</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-16.jpg"><img src="as4/image-16s.jpg"/></a></th>
		<th><a href="as4/input-16.scene">Input 16</a></th>
		<th>970ms</th>
		<th><a href="as4/notes-16">Moving viewport around</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-17.jpg"><img src="as4/image-17s.jpg"/></a></th>
		<th><a href="as4/input-17.scene">Input 17</a></th>
		<th>560ms</th>
		<th><a href="as4/notes-17">Basic ellipsoid test</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-18.jpg"><img src="as4/image-18s.jpg"/></a></th>
		<th><a href="as4/input-18.scene">Input 18</a></th>
		<th>11960ms</th>
		<th><a href="as4/notes-18">Full ellipsoid in scene</a></th>
	</tr>
		<tr>
		<th><a href="as4/image-19.jpg"><img src="as4/image-19s.jpg"/></a></th>
		<th><a href="as4/input-19.scene">Input 01</a></th>
		<th>61540ms</th>
		<th><a href="as4/notes-19">Tons of reflections & spheres</a></th>
	</tr>
	
</table>



	 </td>
      </tr>
    </table>
