import processing.core.*; import java.applet.*; import java.awt.*; import java.awt.image.*; import java.awt.event.*; import java.io.*; import java.net.*; import java.text.*; import java.util.*; import java.util.zip.*; import javax.sound.midi.*; import javax.sound.midi.spi.*; import javax.sound.sampled.*; import javax.sound.sampled.spi.*; import java.util.regex.*; import javax.xml.parsers.*; import javax.xml.transform.*; import javax.xml.transform.dom.*; import javax.xml.transform.sax.*; import javax.xml.transform.stream.*; import org.xml.sax.*; import org.xml.sax.ext.*; import org.xml.sax.helpers.*; public class Bezier extends PApplet {//Drawing Bezier curves!
 
float bezierStep = 0.001f;
boolean showContP = true;
boolean boldL = true;

class MyPoint {
  public int x;
  public int y;
  
  MyPoint() {}
  MyPoint(int x, int y) {this.x=x; this.y=y;}
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
}  
     
MyBezier[] curves = new MyBezier[0];     
     
public void setup() {
  size(300, 300); 
  background(255);  
  addCurve();
}

public void draw() {
  background(255);
  for (int i=0; i<curves.length; i++) { 
    curves[i].draw(showContP);
  }
}

//Moving the control points:

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

//Adding new curves.

public void keyTyped() {
  switch (key) {
   case 'n':
     addCurve();
     break;
   case 'v':
     showContP = !showContP;
     break;
   case 'b':
     boldL = !boldL;
     break;
  }
}

public void addCurve() {
  curves = (MyBezier[]) expand(curves, curves.length+1);
  curves[curves.length-1] = new MyBezier();
  curves[curves.length-1].P[0] = new MyPoint(10,10);
  curves[curves.length-1].P[1] = new MyPoint(10,290);
  curves[curves.length-1].P[2] = new MyPoint(290,290);
  curves[curves.length-1].P[3] = new MyPoint(290,10); 
}

  static public void main(String args[]) {     PApplet.main(new String[] { "Bezier" });  }}