    <td class="main_content_box">

<table width="100%" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td class="left_content">

<i>Negative Space Follows...</i>

	</td>
        <td class="body_content">

<h2>Class Assignments</h2>

<p>
<h3>Assignment 1</h3>
<br>This is purely some example code, here to help us get our system set up for OpenGL work. I used simple OpenGL polygon drawing routines to draw a little mohawked robot. Here's the result:
<br><img src="images/as1.jpg" />
<br><a href="http://www.wetherobots.com">We The Robots</a> inspired this creation.
</p>

<p>
	<h3>Assignment 2</h3>
	This assignment examines simple shading models - specifically, the Phone Illuminiation Model.
	Source Codde will be posted after the due date. For now, enjoy the pretty pictures!
	
	<p><b>Diffuse Material with Single Light Source</b>
	<br><img src="images/as3_DiffuseOnly.png" width=400 height=300/>
	<br>./shader -kd 0.4 0.8 0.5 -pl 2.0 2.0 1.5 0.1 0.4 0.9
	</p>
	
	<p><b>Specular Material with Single Directional Light Source</b>
	<br><img src="images/as3_SpecularDirectional.png" width=400 height=300/>
	<br>./shader -ks 0.4 0.8 0.5 -dl 2.0 2.0 1.5 0.1 0.4 0.9 -sp 20
	</p>
	
	<p><b>Specular Material with Single Point Light Source</b>
	<br><img src="images/as3_specularPoint.png" width=400 height=300/>
	<br>./shader -ks 0.4 0.8 0.5 -pl 2.0 2.0 1.5 0.1 0.4 0.9 -sp 20
	</p>
	
	<p><b>Specular and Diffuse Material with Single Light Source</b>
	<br><img src="images/as3_spec+diffuse.png" width=400 height=300/>
	<br>./shader -kd 0.4 0.8 0.5 -ks 0.4 0.8 0.5 -pl 2.0 2.0 1.5 0.1 0.4 0.9 -sp 20
	</p>
	
	<p><b>Multiple Light Sources</b>
	<br><img src="images/as3_multipleLights.png" width=400 height=300/>
	<br>./shader -kd 0.7 0.7 0.7 -ks 0.7 0.2 0.2 -pl 4.0 0.0 2.0 0.7 0.1 0.1 -pl 0.0 4.0 2.0 0.4 0.5 0.9 -pl -4.0 -4.0 1.0 0.4 0.8 0.5 -pl 0.0 0.0 1.0 0.4 0.5 0.9	-sp 20.0
	</p>

	<h2>Assignment 2 Extra</h2>
	I also implemented a simple animation of lights - moving the primary light source by either
	increading or decreasing its (x,y) position through keyboard interaction. Unfortunately I do not
	have a nice animated GIF of this up yet...
	<br><b>Bump Mapping</b>
	<br>I hoped to implement actual bump mapping. I did not finish the bump map implementation itself, but wrote
	the hooks for preturbing normals to create a rough surface. A comparison between non-bumpmapped and bumpmapped:
	<br><img src="images/as3_non_bumpmap.png" width=400 height=300/>
	<br><img src="images/as3_with_bumpmap.png" width=400 height=300/>
</p>

<p>
<h2>Assignment 3</h2>
<br>This assignment builds a fully functional ray tracer. Pretty sweet? yea, i know.
</p>

<p>
<h2>First Steps - Writing images!</h2>
<br>Since we need to output pretty things, we need o be able to write pixels to disk somehow. I'm going to go with writing BMP files, since the format is very straightforward. I opted to roll my own code to do this, rather than mess with libraries. This keeps my executable size small, makes linking easy, teaches me more about image format, and frankly only takes about 20 lines. I coded up the basic procedure to write out the headers and pixel data, which i'm storing internally in a dynamically allocated array. Here is my first Blank Screen of Death:
<br><center><img src="images/rt-BSOD.jpg"/> </center>
<br>Yes, it is a little blank box. Wee!
<br>This is a crucial piece of code to get right, so i generated a couple of test images to make sure it works nicely. I iterated over all the pixels in the image, alternatively moving from 0 to 255 color values along the x and y values, alternating between red, green and blue. The output was exactly what I expected it to be, so at least this part works!
<br><img src="images/rt-redgreen.jpg"/>&nbsp<img src="images/rt-bluegreen.jpg"/>

<p>
I decided to implement many of the less "glamorous" features before moving on to outputting fancy graphics. I designed my raytracer as a whole (documented in the README file I will post further down) and implemented .OBJ file parsing (which I slightly extended to support my definition of spheres and lights), argument parsing, Scene generation (which consists of an eye, viewport, lights and primitives), Sampling the viewport and aggregating these samples into a film, which sets output to my image library.
This took quite a while, as you can imagine, and I wrote unit tests and significant debugging hooks. I implemented 5 levels of debugging printout, as well as error and info output, which logs a timestamp, function, file and line number. I tested as I implemented to ensure my code works as I expect it to. Finally I could parse full OBJ files (see the teapot.obj posted under examples), create all the necessary memory structures, and set it up ready for raytracing. So far, I had no cool output to show yet, thus I started working on ray intersection code around 2am today. It didn't take long to get basic sphere intersection going, and here's the fruits of my labors. As you can see, I can already instantiate multiple copies of a sphere, and move my camera perspective around in all kinds of cool ways:
<br><img src="images/rt-1firstsphere.jpg" width=400 height=200/>
<br><img src="images/rt-2multispheres.jpg" width=400 height=400/>
<br><img src="images/rt-3cameraperspective.jpg" width=400 height=400/>
<br>
<br>After getting a good framework in place, I was ready to hammer out the core part of the raytracer itself - intersection testing and ray casting. My ray casting procedure itself was fairly easy, since I could recycle all my shading code from the previous assignment, and I already had routines to calculate vector reflections and sphere normals. I used this to create shadow rays by casting a ray to each light, and have that affect the shading calculation. Reflections is done by calculating a reflection vector around the normal of the surface, and recursively calling my raytracing procedure on this new ray.
<br>Ambient Shading was the first to be implemented and thoroughly tested, since this proved that my raytracing and intersection tests were working just fine:
<br><img src="images/rt-4ambientcolors.jpg" width=400 height=400/>
<br>I realized that I had a bug in finding the intersection point that did not guarantee the correct order along the ray, so after a slight modification I could draw overlapping spheres:
<br><img src="images/rt-5overlapping.jpg" width=400 height=400/>
<br>I made the necessary modifications for my shading code from the previous assignment to work with the rays I have available:
<br><img src="images/rt-6shading.jpg" width=400 height=400/>
<br>Two more modifications - one to cast rays to the light and check for an occluder, and one to recursively shade reflection rays. I'm extremely excited at this point:
<br><img src="images/rt-7reflectionsshadows.jpg" width=400 height=400/>
<br>I wrote a little ruby script to generate environments for me, and here's a nice image with 6x6x6 cube of spheres, all reflecting off each other:
<br><img src="images/rt-8many.jpg" width=400 height=400/>
</p>
</p>



	 </td>
      </tr>
    </table>
