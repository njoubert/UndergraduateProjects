import Jama.util.*;
import Jama.*;
import Jama.test.*;
import Jama.examples.*;

float kS = 0.4; //diffusion constant
float aS = 0.4; //dissipation rate
float visc = 0.5; //viscosity

int dim = 2;
int n = 4; //amount of cells per dimension.
int [] l = {100,100}; //length of the grid along each coordinate axis;
float dt = 0.04;


FluidCell [][] u0 = new FluidCell[n][n];
FluidCell [][] u1 = new FluidCell[n][n];
SubstanceCell [][] s0 = new SubstanceCell[n][n];
SubstanceCell [][] s1 = new SubstanceCell[n][n];
  
class SubstanceCell {

}

class FluidCell {
  FluidCell() {
     u = new Matrix(2,1); //velocity vector;
     u.set(0,0,0.5);
     u.set(1,0,0.5); 
  }
  Matrix u;
  
}

void stepV(Matrix f) {
  f.times(dt);
  //For each cell, Add Force
  for (int i=0; i<n; i++) {
   for (int j=0; j<n; j++) {
      u0[i][j].u.plusEquals(f);
   } 
  }
  //For each cell, Transport
  
  //For each cell, Diffuse
  
  
}

void setup() {
 size(400,400);
 frameRate(25);
 
 for (int i=0; i<n ; i++)
   for (int j=0; j<n ; j++) {
     u0[i][j] = new FluidCell();
     s0[i][j] = new SubstanceCell();
     u1[i][j] = new FluidCell();
     s1[i][j] = new SubstanceCell();     
   }
     
}

void draw() {
  background(255);
 
 Matrix f = new Matrix(2,1);
 f.set(0,0,0.005);
 f.set(1,0,0.0);
 
 
 //Swap u0, u1
 FluidCell [][] tempF = u0;
 u0 = u1;
 u1 = tempF;
 //Swap s0, s1
 SubstanceCell [][] tempS = s0;
 s0 = s1;
 s1 = tempS;
 
 //Step Velocity
 stepV(f);
 //Step Substance
 
 int ox = 0;
 int oy = 0;
  for (int i=0; i<n ; i++)
   for (int j=0; j<n ; j++) {
     
  
     
   }
     
 
  
}
