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

	 </td>
      </tr>
    </table>
