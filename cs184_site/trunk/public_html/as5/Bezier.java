import processing.core.*; import java.lang.StringBuffer; import java.applet.*; import java.awt.*; import java.awt.image.*; import java.awt.event.*; import java.io.*; import java.net.*; import java.text.*; import java.util.*; import java.util.zip.*; import javax.sound.midi.*; import javax.sound.midi.spi.*; import javax.sound.sampled.*; import javax.sound.sampled.spi.*; import java.util.regex.*; import javax.xml.parsers.*; import javax.xml.transform.*; import javax.xml.transform.dom.*; import javax.xml.transform.sax.*; import javax.xml.transform.stream.*; import org.xml.sax.*; import org.xml.sax.ext.*; import org.xml.sax.helpers.*; public class Bezier extends PApplet {/** 
 			<p>
				<u>Keyboard Commands:</u>
				<br><b>v</b> - toggle control points
				<br><b>b</b> - toggle line weight
				<br><b>n</b> - add a new curve to the scene
				<br><b>r</b> - reset the scene
				<br><b>1</b> - load example 1
			</p>
  <p>
  <a rel="license" href="http://creativecommons.org/licenses/by-sa/3.0/us/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-sa/3.0/us/80x15.png"/></a><br/>This work is licensed under a Creative Commons <br><a rel="license" href="http://creativecommons.org/licenses/by-sa/3.0/us/">Attribution-Share Alike 3.0 United States License</a>.
  </p>
*/



String ex1 = "(69,93) (10,290) (197,291) (249,94)\n(69,92) (133,51) (263,56) (250,95)\n(69,92) (86,136) (223,127) (248,95)\n(69,93) (116,-7) (269,-11) (250,96)\n(86,136) (78,175) (168,140) (87,137)\n(153,146) (152,182) (240,133) (154,146)\n(84,191) (87,219) (146,218) (156,199)";

MyBezier[] curves;     
float bezierStep = 0.001f;
boolean showContP = true;
boolean boldL = true;

class MyPoint {
  public int x;
  public int y;
  
  MyPoint() {}
  MyPoint(int x, int y) {this.x=x; this.y=y;}
  MyPoint(String d) {
     String[] p = split(d, ",");
     x = PApplet.parseInt(p[0].substring(1));
     y = PApplet.parseInt(p[1].substring(0,p[1].length()-1));
  }
  public void draw(boolean bold) {
    if (!bold)
      point(x,y);
    else
      rect(x-2,y-2,4,4);
  }
  //Returns true if the given x,y is within dist of point
  public boolean within(int x, int y, int dist) {
    return (abs(this.x-x) < dist && abs(this.y-y) < dist);
  }
  
  public String toString() {
    return "(" + x + "," + y + ")"; 
  }
}  
     
class MyBezier {
  MyPoint[] P = new MyPoint[4];
  int myc;
  MyBezier() {
   P[0] = new MyPoint();
   P[1] = new MyPoint();
   P[2] = new MyPoint();
   P[3] = new MyPoint();
   myc = color(random(200),random(200),random(200));
  }  
  public void draw(boolean showControlPoints) {
    stroke(myc);
    for (float t=0; t<1; t+= bezierStep) {
       calcPoint(t).draw(boldL);    
    }
    if (showControlPoints) {
      P[0].draw(true);
      P[1].draw(true);
      P[2].draw(true);
      P[3].draw(true); 
    }
  }
  
  private MyPoint calcPoint(float t) {
    MyPoint p = new MyPoint();
    float x = P[0].x*pow((1-t),3) + 
          P[1].x*3*t*pow((1-t),2) +
          P[2].x*3*pow(t,2)*(1-t) +
          P[3].x*pow(t,3);
    float y = P[0].y*pow((1-t),3) + 
          P[1].y*3*t*pow((1-t),2) +
          P[2].y*3*pow(t,2)*(1-t) +
          P[3].y*pow(t,3);
    
    p.x = (int) x;
    p.y = (int) y;
    return p;
  }
  
  public String toString() {
    return P[0].toString() + " " +P[1].toString() + " " +P[2].toString() + " " +P[3].toString() + "\n"; 
  }
}  
     
public void setup() {
  size(300, 300); 
  background(255);  
  setUpCurvesFromData(ex1);
}

public void draw() {
  background(255);
  for (int i=0; i<curves.length; i++) { 
    curves[i].draw(showContP);
  }
}

// Moving the control points:

boolean moving = false;
MyPoint movingP;

public void mousePressed() {
  for (int i = 0; i < curves.length; i++) {
    for (int p=0;p<4;p++) {
      if (curves[i].P[p].within(mouseX, mouseY, 8)) {
        moving=true;
        movingP = curves[i].P[p]; 
      }
    }
  }
}

public void mouseDragged() {
  if (moving) {
    movingP.x = mouseX;
    movingP.y = mouseY;
    redraw();
  }
}

public void mouseReleased() {
  moving = false;  
}

// Handling Keyboard Input

public void keyTyped() {
  switch (key) {
   case 'n':               //Add a curve
     addCurve();
     break;
   case 'v':               //Toggle handles
     showContP = !showContP;
     break;
   case 'b':               //Toggle Bold
     boldL = !boldL;
     break;
   case 'r':               //Reset curves
     resetCurves();
     addCurve();
     break;
   case 'o':               //Output Curves
     getData();
     break;
   case 'i':               //Input curves
   
    break;   
   case '1':               //example 1
     setUpCurvesFromData(ex1);
     break;
   
  }
}

// Adding new curves.

public void resetCurves() {
  curves = new MyBezier[0];
}

public void addCurve() {
  curves = (MyBezier[]) expand(curves, curves.length+1);
  curves[curves.length-1] = new MyBezier();
  curves[curves.length-1].P[0] = new MyPoint(10,10);
  curves[curves.length-1].P[1] = new MyPoint(10,290);
  curves[curves.length-1].P[2] = new MyPoint(290,290);
  curves[curves.length-1].P[3] = new MyPoint(290,10); 
}

public void addCurve(String data) {
  curves = (MyBezier[]) expand(curves, curves.length+1);
  String el[] = split(data, " ");
  curves[curves.length-1] = new MyBezier();
  curves[curves.length-1].P[0] = new MyPoint(el[0]);
  curves[curves.length-1].P[1] = new MyPoint(el[1]);
  curves[curves.length-1].P[2] = new MyPoint(el[2]);
  curves[curves.length-1].P[3] = new MyPoint(el[3]); 
}

// I/O

public String getData() {
  StringBuffer data = new StringBuffer();
  for (int i = 0; i < curves.length; i++) {
   data.append(curves[i].toString());
  }
  print(data.toString());
  return data.toString();
}

public void setUpCurvesFromData(String data) {
   String el[] = split(data, "\n");
   print("Data contains # curves: ");
   println(el.length);
   resetCurves();
   for (int i = 0; i < el.length; i++) {
     addCurve(el[i]); 
   }
   
}

  static public void main(String args[]) {     PApplet.main(new String[] { "Bezier" });  }}