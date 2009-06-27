import processing.core.*; 
import processing.xml.*; 

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

public class FractalMtn1 extends PApplet {

class Camera {
 		public float _zoom = 1000.0f;
		public float _rotx = PI/4;
		public float _roty = PI/4;
		public float _tx = 0.0f;
		public float _ty = 0.0f; 

  Camera() { }
  
  	public void setView() {
		resetMatrix();
                camera(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		//This order gives you Maya-like
		//mouse control of your camera
		translate(0, 0, -_zoom);
		translate(_tx, _ty, 0);
		rotateX(-_rotx);
		rotateY(_roty);
	}

  public void mouseDragged() {
    float diffx = mouseX - pmouseX;
    float diffy = mouseY - pmouseY;
    if (mouseButton == LEFT) {
     _rotx += 0.005f * diffy;
     _roty += 0.005f * diffx;
    } else if (mouseButton == RIGHT) {
     _tx += 0.0005f * diffx * (abs(_zoom) + 1);
     _ty += 0.0005f * diffy * (abs(_zoom) + 1);
    } else if (mouseButton == CENTER) {
     _zoom -= 0.01f * diffx * (abs(_zoom) + 1);
    } 
    
  }

  
}


class Point {
 public float x, y, z;
 boolean hasBeenDeviated;
 Point(Point a, Point b) {
   this.x = (a.x + b.x)/2; 
   this.y = (a.y + b.y)/2; 
   this.z = (a.z + b.z)/2;
   this.hasBeenDeviated = false;
 }
 Point(float x, float y, float z) {
  this.x = x;
  this.y = y; 
  this.z = z;

  this.hasBeenDeviated = true;
 } 
 public void setZ(float z) {
   this.z = z;
   hasBeenDeviated = true;
 }
 public boolean equalsNoZ(Point other) {
  return abs(this.x - other.x) < 0.001f && 
    abs(this.y - other.y) < 0.001f;
 }
}

class Triangle {
  public int p0, p1, p2;
  Triangle(int x0, int x1, int x2) {
    this.p0 = x0;
    this.p1 = x1;
    this.p2 = x2;
  }
}

class TriangleNode {
  Triangle val;
  TriangleNode next;
  TriangleNode(Triangle val, TriangleNode next) {
   this.val = val;
   this.next = next;
  } 
}

class TriangleLinkedList {
  public TriangleNode root;
  TriangleLinkedList() {
   root = null;
  } 
}

//---------------------------------------------------------------------------------------------
//=============================================================================================
//=============================================================================================
//=============================================================================================

TriangleLinkedList triangles = new TriangleLinkedList();
Point[] points = new Point[10];
int pointSize = 10;
int pointLen = 0;
static final int MAX_PASSES = 6;
static final float ROUGHNESS = 1.43f; //0.0 to 5.0 as an example range.

Camera cam = new Camera();

//=============================================================================================
//=============================================================================================
//=============================================================================================
//---------------------------------------------------------------------------------------------

public int addPoint(Point p) {
    if (pointLen == pointSize) {
      pointSize *= 2;
      points = (Point[]) expand(points, pointSize);
     }
    points[pointLen] = p;
    pointLen++;
    return pointLen-1;  
}

public int getOrCreatePoint(Point a, Point b) {
  //first we try to find a point that's between these two.
  Point newPoint = new Point(a,b);
 for(int i = 0; i < pointLen; i++) {
  if (points[i].equalsNoZ(newPoint))
    return i;
 }
 
 //if not, we add one.
 return addPoint(newPoint); 
}

//---------------------------------------------------------------------------------------------

public void addTriangle(TriangleLinkedList llist, Triangle t) {
  if (llist.root == null) {
   llist.root = new TriangleNode(t, null); 
  } else {
    TriangleNode cur = llist.root;
    while (cur.next != null) {
     cur = cur.next;
    }
    cur.next = new TriangleNode(t, null);
  }
}
public void delTriangle(TriangleLinkedList llist, Triangle t) {
  if (llist.root.val == t) {
    llist.root = llist.root.next;
  } else {
    TriangleNode cur = llist.root;
    while (cur.next != null && cur.next.val != t) {
     cur = cur.next;
    } 
    if (cur.next == null)
      System.out.println("WTF!");
    else
     cur.next = cur.next.next;
  }
}

//---------------------------------------------------------------------------------------------


public void deviatePoint(int p, int iter) {
  if (points[p].hasBeenDeviated) {
   return; 
  }
  int iternr = (MAX_PASSES) - iter;
  float dev = pow((iternr*8), ROUGHNESS);
  points[p].setZ(points[p].z + random(-dev,dev));
}

public void subdivideAndFractalize(TriangleLinkedList llist, Triangle t, int iter) { //root to save to given
  int p0 = t.p0;
  int p1 = t.p1;
  int p2 = t.p2;
  
  int p01 = getOrCreatePoint(points[p0], points[p1]);
  int p02 = getOrCreatePoint(points[p0], points[p2]);
  int p12 = getOrCreatePoint(points[p1], points[p2]);
  
  deviatePoint(p01, iter);
  deviatePoint(p02, iter);
  deviatePoint(p12, iter);
  
  delTriangle(triangles, t);
  
  addTriangle(llist, new Triangle(p0, p01, p02));
  addTriangle(llist, new Triangle(p02, p12, p01));
  addTriangle(llist, new Triangle(p02, p2, p12));
  addTriangle(llist, new Triangle(p01, p12, p1));
  
  
}

public void fractalMountainGenerator() {
  for (int passes = 0; passes < MAX_PASSES; passes++) {
    TriangleLinkedList newT = new TriangleLinkedList();
    TriangleNode cur = triangles.root;
    while(cur != null) {
       //For each triangle in this pass:
      subdivideAndFractalize(newT, cur.val, passes); 
      cur = cur.next;
    }
    triangles = newT; //save the new array
  }
}

public void flattenBottoms() {
 for (int i = 0; i < pointLen; i++) {
  if (points[i].z < 0)
    points[i].z = 0;
 } 
}

//---------------------------------------------------------------------------------------------

public void setup() {
   size(800,600,P3D);
   int p0 = addPoint(new Point(0,0,0));
   int p1 = addPoint(new Point(600,0,0));
   int p2 = addPoint(new Point(300,447,0));
   int p3 = addPoint(new Point(900,447,0));
   addTriangle(triangles, new Triangle(p0,p1,p2));
   addTriangle(triangles, new Triangle(p1,p2,p3));
   fractalMountainGenerator();
   flattenBottoms();
}

public void setColor(int p) {
    if (points[p].z <= 0) { //water
      specular(0,0,200);
      ambient(0,0,255);
      fill(0,0,255);
    } else if (points[p].z > 200) { //snow
      specular(255,255,255);
      ambient(255,255,255);
      fill(255,255,255);
    } else { //green
      specular(0,200,0);
      ambient(0,255,0);
      fill(0,255,0);
    } 
}

public void draw() {
  background(0);
  noStroke();

  cam.setView();          
  
  rotateX(PI/2);
  translate(-width/2, -height/2);
  shininess(2);
  
    int Pr = 50;
  int Pg = 40;
  int Pb = 40;
  ambientLight(40,40,40);
  pointLight(Pr, Pg, Pb, -200, -200, 600); // Position
  pointLight(Pr, Pg, Pb, 800, 0, 500); // Position
  pointLight(Pr, Pg, Pb, 300, 447, 500); // Position
  pointLight(Pr, Pg, Pb, 900, 447, 500); // Position
  pointLight(Pr, Pg, Pb, 300, 400, 500); // Position
  
  TriangleNode cur = triangles.root;
  while (cur != null) {
    beginShape(TRIANGLES);
    setColor(cur.val.p0);
    vertex(points[cur.val.p0].x,points[cur.val.p0].y,points[cur.val.p0].z);
    setColor(cur.val.p1);
    vertex(points[cur.val.p1].x,points[cur.val.p1].y,points[cur.val.p1].z);
    setColor(cur.val.p2);
    vertex(points[cur.val.p2].x,points[cur.val.p2].y,points[cur.val.p2].z);
    endShape(); 
    cur = cur.next;
  }
}

public void mouseDragged() {
 cam.mouseDragged(); 
}

  static public void main(String args[]) {
    PApplet.main(new String[] { "--bgcolor=#ffffff", "FractalMtn1" });
  }
}
