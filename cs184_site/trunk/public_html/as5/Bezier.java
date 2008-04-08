import processing.core.*; import java.applet.*; import java.awt.*; import java.awt.image.*; import java.awt.event.*; import java.io.*; import java.net.*; import java.text.*; import java.util.*; import java.util.zip.*; import javax.sound.midi.*; import javax.sound.midi.spi.*; import javax.sound.sampled.*; import javax.sound.sampled.spi.*; import java.util.regex.*; import javax.xml.parsers.*; import javax.xml.transform.*; import javax.xml.transform.dom.*; import javax.xml.transform.sax.*; import javax.xml.transform.stream.*; import org.xml.sax.*; import org.xml.sax.ext.*; import org.xml.sax.helpers.*; public class Bezier extends PApplet {//Drawing Bezier curves!
  
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
    if (abs(this.x-x) < dist && abs(this.y-y) < dist)
      return true;
    return false;
  }
}  
     
class MyBezier {
  MyPoint[] P = new MyPoint[4];
  MyBezier() {
   P[0] = new MyPoint();
   P[1] = new MyPoint();
   P[2] = new MyPoint();
   P[3] = new MyPoint();
  }  
  public void draw(boolean showControlPoints) {
    MyPoint p;
    
    stroke(0);
    for (float t=0; t<1; t+= 0.001f) {
       calcPoint(t).draw(false);    
    }
    if (showControlPoints) {
      stroke(255,0,0);
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
     
MyBezier[] curves = new MyBezier[1];     
     
public void setup() {
  size(300, 300); 
  background(255);  
  curves[0] = new MyBezier();
  curves[0].P[0] = new MyPoint(10,10);
  curves[0].P[1] = new MyPoint(10,290);
  curves[0].P[2] = new MyPoint(290,290);
  curves[0].P[3] = new MyPoint(290,10);  
}

public void draw() {
  background(255);
  for (int i=0; i<curves.length; i++) { 
    curves[i].draw(true);
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
  static public void main(String args[]) {     PApplet.main(new String[] { "Bezier" });  }}