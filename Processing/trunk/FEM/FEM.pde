import Jama.util.*;
import Jama.*;

Fem fem;

class Fem {
  Fem() {}
  public Vertex [] vertices;
  public Triangle [] triangles;

  public void draw() {
   for (int i = 0; i < 17; i++) {
    triangles[i].draw(this);
   } 
  }
}

class Triangle {
  Triangle(int a, int b, int c) {
    v1 = a; v2 = b; v3 = c;
  }
  void draw(Fem fem) {
    point(fem.vertices[a].x.get(0,0),fem.vertices[a].x.get(0,1));
    point(fem.vertices[b].x[0],fem.vertices[b].x[1]);
    point(fem.vertices[c].x[0],fem.vertices[c].x[1]);
    line(fem.vertices[a].x[0],fem.vertices[a].x[1],
      fem.vertices[b].x[0],fem.vertices[b].x[1]);
    line(fem.vertices[b].x[0],fem.vertices[b].x[1],
      fem.vertices[c].x[0],fem.vertices[c].x[1]);
    line(fem.vertices[c].x[0],fem.vertices[c].x[1],
      fem.vertices[a].x[0],fem.vertices[a].x[1]);
  }
  int v1, v2, v3;
}

class Vertex {
 Vertex(int xi, int yi) {
  u[0] = x[0] = xi;
  u[1] = x[1] = yi; 
 }
  Matrix u = new Matrix(2,1);
  Matrix x = new Matrix(2,1);
}


void setup() {

  size(400,400);
  background(255);
  

  fem = new Fem();
 
fem.vertices = new Array(17);
for (int i = 0; i < 6; i++) { vertices[i] = new Vertex(i*10, 0); }
for (int i = 0; i < 5; i++) { vertices[i+6] = new Vertex(i*10 + 5, 10); }
for (int i = 0; i < 6; i++) { vertices[i+11] = new Vertex(i*10, 20); }

}

//-----------------------------------------------------------------------

void draw() {
 fem.draw(); 
}
