    <td class="main_content_box">

<table width="100%" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td class="left_content">

<i>Negative Space Follows...</i>

	</td>
        <td class="body_content">

<h2>Assignment 5</h2>

<p><h3>Drawing Bezier Curves</h3>
<br>To get a feel for Bezier curves, I put together a little Processing app that draws Bezier curves. Drag the control points around to change the shape of the curve. Feel free to check out the source code.
<br><iframe src="as5/Bezier/index.html" width=420 height=550 frameborder=0 scrolling="no"></iframe>

</p>
<p><h3>Calculating Bezier Surface points</h3>
<br>Since I could now calculate points on a curve, it was a logical step to attempt finding points on a surface. This example draws the Teapot example surface in a point-based rendering methodology by finding points on surfaces and plotting them. This is purely to experiment with reading in the bez file, and calculating points (and derivates) across the surface.
<br><iframe src="as5/BezierSurface/index.html" width=420 height=500 frameborder=0 scrolling="no"></iframe>
</p>

<p><h3>Now we code...</h3>
<br/>We hit some snags, like <a href="as5/Images/badassmemoryusage.png">running out of virtual memory</a>, but soon enough the project was working!	
</p>

<p><h3>Test Patch! Uniform Subdivision</h3>
<br/><code>./cs184_as5 ../test.bez 0.5</code>

<table>
<tr>
	<td>Smooth Shading</td>
	<td>Flat Shading</td>
</tr>
<tr>
	<td><img src="as5/Images/test_uniform_smooth.png"></td>
	<td><img src="as5/Images/test_uniform_flat.png"></td>
</tr>
<tr>
	<td>Wireframe</td>
	<td>Wireframe with black normals, green derivatives</td>
</tr>
<tr>
	<td><img src="as5/Images/test_uniform_wireframe.png"></td>
	<td><img src="as5/Images/test_uniform_wireframewithdn.png"></td>
</tr>
</table>

<br/>
<br/>

</p>

<p><h3>Test Patch! Adaptive Subdivision</h3>
<br/><code>./cs184_as5 ../test.bez 0.005</code>


</p>

<p><h3>Teapot! Uniform Subdivision</h3>
<br/><code>./cs184_as5 ../test.bez 0.1</code>


</p>

<p><h3>Teapot! Adaptive Subdivision</h3>
<br/><code>./cs184_as5 ../test.bez 0.01</code>


</p>


	 </td>
      </tr>
    </table>
