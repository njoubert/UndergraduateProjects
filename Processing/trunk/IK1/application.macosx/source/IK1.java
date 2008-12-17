import processing.core.*; 
import processing.xml.*; 

import Jama.util.*; 
import Jama.*; 
import Jama.test.*; 
import Jama.examples.*; 

import java.applet.*; 
import java.awt.*; 
import java.awt.image.*; 
import java.awt.event.*; 
import java.io.*; 
import java.net.*; 
import java.text.*; 
import java.util.*; 
import java.util.zip.*; 
import java.util.regex.*; 

public class IK1 extends PApplet {

/*
Inverse Kinematics.
Press 0 to enable forward kinematics mode.
Press 1 to enable inverse kinematics mode.
Press 2 to enable animated inverse kinematics mode.
Drag the sliders to change joint angles.
Move the root and goal points (red and yellow circles) to change inverse kinematics behaviour.
*/







int dim = 2;         //2D system
int numJoints = 3;   //with 3 joints

int mode = 0;





/******************************************************************************************
 *
 *  INVERSE KINEMATICS CALCULATIONS
 *
 ******************************************************************************************/


public float calculateEndpointX(float th1, float th2, float th3, int l1, int l2, int l3) {
  float xpos = skel.root.home.x + l1*sin(th1) + l2*sin(th1 + th2) + l3*sin(th1 + th2 + th3);
  return xpos;
}

public float calculateEndpointY(float th1, float th2, float th3, int l1, int l2, int l3) {
  float ypos = skel.root.home.y + l1*cos(th1) + l2*cos(th1 + th2) + l3*cos(th1 + th2 + th3);
  return ypos;
}


Matrix J;

//Calculates the jacobian of each coordinate with respect to each angle, saves it in J.
//Can also do this with finite differences...
public void calculateJacobian() {
  float th1 = skel.root.theta;
  float th2 = skel.root.outbound.child.theta;
  float th3 = skel.root.outbound.child.outbound.child.theta;
  int l1 = skel.root.outbound.length;
  int l2 = skel.root.outbound.child.outbound.length;
  int l3 = skel.root.outbound.child.outbound.child.outbound.length;
  
  J.set(0,0,l1*cos(th1) + l2*cos(th1+th2) + l3*cos(th1+th2+th3));
  J.set(1,0,-l1*sin(th1) - l2*sin(th1+th2) - l3*sin(th1+th2+th3));

  J.set(0,1,l2*cos(th1+th2) + l3*cos(th1+th2+th3));
  J.set(1,1,-l2*sin(th1+th2) - l3*sin(th1+th2+th3));
  
  J.set(0,2,l3*cos(th1+th2+th3));
  J.set(1,2,-l3*sin(th1+th2+th3));  
}

public void calculateNewAngles(boolean smallStep, int goalx, int goaly) {
  float newth[] = new float[numJoints];
  
  float errorThresh = 0.0001f;
  
  float th1 = skel.root.theta;
  float th2 = skel.root.outbound.child.theta;
  float th3 = skel.root.outbound.child.outbound.child.theta;
  int l1 = skel.root.outbound.length;
  int l2 = skel.root.outbound.child.outbound.length;
  int l3 = skel.root.outbound.child.outbound.child.outbound.length;
  //Calculate current position
  float xpos = calculateEndpointX(th1,th2,th3,l1,l2,l3);
  float ypos = calculateEndpointY(th1,th2,th3,l1,l2,l3);

  float dx = goalx - xpos;
  float dy = goaly - ypos;
  float currentError = dx*dx + dy*dy;
  
  //Set up deltaP:
  Matrix deltaP = new Matrix(2, 1);
  deltaP.set(0, 0, dx);
  deltaP.set(1, 0, dy);
  
  //Calculating pseudoinverse of J (from Jimmy)
      Matrix J_transpose = J.transpose();
      SingularValueDecomposition svd = J_transpose.svd(); // svd only works on m>n matrix; so use J^T instead of J
      // get the needed matrices out of the svd (keep in mind, it's the svd of J^T not J)
      Matrix U = svd.getU();
      Matrix S = svd.getS();
      Matrix V_transpose = svd.getV().transpose();
      // invert S, with threshold
      for (int i = 0; i < S.getRowDimension(); i++)
      {
        double val = S.get(i,i);
        if (abs((float)val) < errorThresh)
          val = 0;
         else
          val = 1/val;
        
        S.set(i,i,val);
      }
      Matrix pseudoInv = U.times(S.times(V_transpose));      // this is J^-P
      
      //Now solve for the needed change in thetas.
      Matrix c = pseudoInv.times(deltaP);
      
      //For now we just take the full step all the time:
      float offs[] = new float[numJoints];
      for (int i = 0; i < numJoints; i++) {
        offs[i] = (float)c.get(i, 0);
        if (mode > 1)
          offs[i] *= 0.04f;
      }
      
      int count = 0;
      while(count < 30) {
        newth[0] = skel.root.theta + offs[0];
        newth[1] = skel.root.outbound.child.theta + offs[1];
        newth[2] = skel.root.outbound.child.outbound.child.theta + offs[2];    
        for (int i = 0; i < numJoints; i++) {
          while (newth[i] > TWO_PI) {
          newth[i] -= TWO_PI;
        } 
          while(newth[i] < 0) {
             newth[i] += TWO_PI; 
          }
        }
        float xnew = calculateEndpointX(newth[0],newth[1],newth[2],l1,l2,l3);
        float ynew = calculateEndpointY(newth[0],newth[1],newth[2],l1,l2,l3);
        float dxn = goalx - xnew;
        float dyn = goaly - ynew;
        float newErr = dxn*dxn + dyn*dyn;
        if (newErr <= currentError) {
          skel.root.theta = newth[0];
          skel.root.outbound.child.theta = newth[1];
          skel.root.outbound.child.outbound.child.theta = newth[2]; 
          break; 
        }
        for (int i = 0; i < numJoints; i++) {
          offs[i] *= 0.5f;
        } 
        count++; 
      }
      
      
}







/******************************************************************************************
 *
 *  KINEMATICS SETUP. Tree structure to represent connections between joints and bones.
 *
 ******************************************************************************************/







class Point {
  Point(int xi, int yi) { 
    x = xi; 
    y = yi; 
  }
  Point() {
    
  }
  public Point copy() { return new Point(x, y); }
  int x, y;
}

class Bone {
   Bone(int l) { length = l; }
   int length;
   Joint parent;
   Joint child;
   public Point getEnd() {
       Point start = parent.getPoint();
       float theta = parent.getTheta();
       start.x += length*sin(theta);
       start.y += length*cos(theta);
       return start;
   }
   public void draw() {
     Point s = parent.getPoint();
     Point e = getEnd();
     //println("S: " + s.x + "," + s.y + " E: " + e.x + "," + e.y);
     line(s.x, s.y, e.x, e.y);
     if (child != null)
       child.draw();
   }
   public void addJoint() {
      child = new Joint();
      child.inbound = this; 
   }
   
}

class Joint {
  float theta; //rotation in RADIANS
  Bone inbound;
  Bone outbound;
  public Point getPoint() {
    return inbound.getEnd();
  }
  public float getTheta() {
    float r = theta; 
    if (inbound != null)
       r += inbound.parent.getTheta();
     return r;
  }
  public void draw() {
    Point p = getPoint();
    ellipse(p.x, p.y, 10, 10);
    if (outbound != null)
      outbound.draw();
  }
  public void addBone(int l) {
     outbound = new Bone(l);
     outbound.parent = this;
  }
}

class RootJoint extends Joint {
  RootJoint() {
   home = new Point();
   home.x = 240;
   home.y = 390; 
  }
  public Point getPoint() {
    return home.copy();
  }
  Point home;
}

class Skeleton {
   public Skeleton() {
     root = new RootJoint(); 
   }
   RootJoint root; 
   public Point getEndpoint() {
      float th1 = root.theta;
      float th2 = root.outbound.child.theta;
      float th3 = root.outbound.child.outbound.child.theta;
      int l1 = root.outbound.length;
      int l2 = root.outbound.child.outbound.length;
      int l3 = root.outbound.child.outbound.child.outbound.length;
      float px = root.home.x + l1*sin(th1) + l2*sin(th1 + th2) + l3*sin(th1 + th2 + th3);
      float py = root.home.y + l1*cos(th1) + l2*cos(th1 + th2) + l3*cos(th1 + th2 + th3); 
      return new Point((int)px,(int)py);  
   }
   public void draw() {
    root.draw();
  
    Point p = getEndpoint();
    stroke(255,0,0);
    ellipse(root.home.x, root.home.y, 10, 10);
  
    stroke(0, 255, 0);
    ellipse(p.x, p.y, 10, 10); 
  }
}

Skeleton skel = new Skeleton();
Point goal = new Point(420,200);

public void setup() { 
  size(700,400); 
  skel.root.theta = radians(170);
  skel.root.addBone(110);
  skel.root.outbound.addJoint();
  skel.root.outbound.child.addBone(110);
  skel.root.outbound.child.outbound.addJoint();
  skel.root.outbound.child.outbound.child.addBone(110);
  
  J = new Matrix(dim, numJoints);
  
  PFont myFont = createFont("FFScala", 12);
  textFont(myFont);

  calculateJacobian();
  calculateNewAngles(false, 300, 400); //To do nice setup...
  
}

public void draw() {
  
  calculateJacobian();
  if (mode > 0)
    calculateNewAngles(true, goal.x, goal.y);
  
  background(50,50,70);
  fill(250);
  stroke(255);
  strokeWeight(4.0f);
  
  //Draw the skeleton
  skel.draw();
  
  //Draw the goal.
  stroke(255,255,0);
  ellipse(goal.x, goal.y, 12, 12);

  pushMatrix();
    //Draw the control box.
    drawControls();
    //text("M x:" + pmouseX + ", y:" + pmouseY, 100, 300);
  popMatrix();
  
  //Draw the Jacobian
  text("Jacobian: ", 10, 350);
  for (int i = 0; i < J.getColumnDimension(); i++) {
    for (int j = 0; j < J.getRowDimension(); j++) {
      text((float)J.get(j,i), 5+i*65, 360+10+j*20);
    }
  }
    
}







/******************************************************************************************
 *
 *  INTERACTIVE CONTROLS OF APPLET
 *
 ******************************************************************************************/






class Slider {
  Slider(int x, int y, int len) { pVal = 0; pStartX = x; pStartY = y; pLength = len; valMin=0; valMax=360;}
  public void draw() {
    //One slider takes up 10 pixels of Y space, and we want to leave 5 pixels blank on either side.
    rect(pStartX+pVal, pStartY, boxLen, boxLen);
    line(pStartX+(boxLen/2), pStartY+(boxLen/2), pStartX+(boxLen/2)+pLength, pStartY+(boxLen/2));
    text("v: " + getVal() , pStartX+25+pLength,  pStartY+10); 
  } 
  public float getVal() {
     return map(pVal, 0, pLength, valMin, valMax);
  }
  public void setVal(float val) {
     pVal = (int) map(val, valMin, valMax, 0, pLength); 
  }
  public boolean didAndHandleHit() {
    if ((abs(pmouseX - (pStartX+(boxLen/2)+pVal)) < (boxLen/2)) && (abs(pmouseY - (pStartY+(boxLen/2))) < (boxLen/2))) {
      int newVal = mouseX - (pStartX+5);
      if (newVal <= pLength && newVal >= 0) {
        updateValue(newVal);
        return true;  
      }
    }
    return false;  
  }
  public void updateValue(int val) {
    pVal = val;
  }
  
   int pStartX;
   int pStartY;
   int pLength;
   int pVal;
   int valMin;
   int valMax;
   int boxLen = 20;
}

Slider s1 = new Slider(5, 175, 100);
Slider s2 = new Slider(5, 200, 100);
Slider s3 = new Slider(5, 225, 100);
  
public void drawControls() {
  fill(255);
  stroke(0);
  rect(0,0,220,400);
  fill(0);
  text("WELCOME TO IK SOLVER!", 15, 20);
  text("Controls:", 15, 35);
  if (mode == 0)
    fill(0,255,0);
  else
    fill(0);
  text("Press 0: Forward Kinematics", 15, 50);
  if (mode == 1)
    fill(0,255,0);
  else
    fill(0);
  text("Press 1: Inverse Kinematics", 15, 65);
  if (mode == 2)
    fill(0,255,0);
  else
    fill(0);
  text("Press 2: Animated IK", 15, 80);
  
  fill(0);
  
  
  text("Drag the circles:", 15, 95);
  text("  RED    = root", 15, 110);
  text("  YELLOW = goal", 15, 125); 
  
  text("Angles of Joints (Drag Sliders):", 15, 160);
  s1.setVal(degrees(skel.root.theta));
  s2.setVal(degrees(skel.root.outbound.child.theta));
  s3.setVal(degrees(skel.root.outbound.child.outbound.child.theta));
  s1.draw();
  s2.draw();
  s3.draw();
}

public void mouseDragged() {
  //Can move the base...
  if (abs(pmouseX - skel.root.home.x) < 10 && abs(pmouseY - skel.root.home.y) < 10) {
   skel.root.home.x = mouseX;
   skel.root.home.y = mouseY;
   return;
  }
  
  if (abs(pmouseX - goal.x) < 10 && abs(pmouseY - goal.y) < 10) {
   goal.x = mouseX;
   goal.y = mouseY;
   return;
  }
  
  if (s1.didAndHandleHit()) {
    skel.root.theta = radians(s1.getVal());
    return; 
  }
  if (s2.didAndHandleHit()) {
    skel.root.outbound.child.theta = radians(s2.getVal());
    return;  
  } 
  if (s3.didAndHandleHit()) {
    skel.root.outbound.child.outbound.child.theta = radians(s3.getVal());
    return;
  }
  
}

public void keyPressed() {
  if (keyCode == 48)
    mode = 0;
  else if (keyCode == 49)
    mode = 1;
  else if (keyCode == 50)
    mode = 2;
}


  static public void main(String args[]) {
    PApplet.main(new String[] { "--bgcolor=#ffffff", "IK1" });
  }
}
