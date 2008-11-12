    <td class="main_content_box">

<table width="100%" border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td class="left_content">

<i>Negative Space Follows...</i>

	</td>
        <td class="body_content">

<h2>Final Project - Hacking Rockband</h2>

<p>
	For the final project for this class I am spearheading a team with the aim of creating computer vision software and a hardware interface that can play the XBox 360 version of Rockband by itself.
</p>

<p><h3>Plan of Attack</h3>
<br/>The project is nicely divided into two sections, hardware output and software control/input. We'll be handling these as two seperate sub-projects.
<br/>For the hardware:
<ul>
	<li>Draft a rough specification of what the hardware needs to accomplish</li>
	<li>Come up with several designs</li>
	<li>Choose two or three feasible ones</li>
	<li>Try to prototype each. Evaluate them according to a set of criteria</li>
	<li>Build the final version.</li>
</ul>
<br>

<h2>Hardware</h2>

<p>
	<h3>Hardware Specification</h3>
	The goal of the hardware is to take input from our computer vision system and generating input for the xbox to simulate a person playing a rockband instrument. I'm leaving this as open-ended as possible by intention - the problem statement is not that complicated, and as long as we can programatically push buttons on the xbox we've accomplished our goal.
</p>

<p>
	<h3>Hardware Designs</h3>
	
	<p>
		<b>A direct USB-to-USB conncetion</b> would be the ideal solution. Have the whole hardware system consist purely of a USB extention cable, computer to xbox. No fuss.
		<br/>Evaluation:
		<ul>
			<li>Total cost - Just a USB cable.</li>
			<li>Expertise Needed - Reverse engineering the XBox USB protocol.</li>
			<li>Expertise Needed - Recreating the protocol in our software.</li>
		</ul>
		<br/>Feasibility:
		<ul>
			<li>Cost - Negligible.</li>
			<li>Interfacing with USB - On first sight this is significantly harder than working with parallel/serial ports (with which I have experience). According to this USB FAQ, this will require special hardware to convert between two "Host" devices, since USB relies on a host-device configuration versus our setup of host(xbox) to host(PC). </li>
			<li>Reverse Engineering - </li>
			<li>Recreating the Protocol - Once</li>
		</ul>
		
	</p>

	<p>
		<b>Hack a controller</b> as an interface between the xbox and the computer. Controllers are reasonably cheap, already talk the XBox USB protocol and are conveniently lined with buttons that (guess what) already control Rockband. Its simply a matter of pushing the buttons.
	</p>

	<p>
		Build a <b>Robotic Arm</b> to play the instrument mechanically. No mods necessary - build as much extra hardware as needed to play the game.
	</p>
</p>



<h2>Software</h2>

<p>
	
</p>

	 </td>
      </tr>
    </table>
