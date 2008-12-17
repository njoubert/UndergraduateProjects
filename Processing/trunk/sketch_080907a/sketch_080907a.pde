Particle[][] particles;


int dist_x = 20;
int dist_y = 20;
int swidth = 400;
int sheight = 400;

void setup() {
  
  size(swidth, sheight);
  smooth();
  fill(0);
  frameRate(30);
  
  float vx = 0.2;
  float vy = 0.2;
  
  particles = new Particle[dist_x][dist_y];
  for (int i = 0; i < dist_x; i++) {
    for (int j = 0; j < dist_y; j++) {
     particles[i][j] = new Particle();
     particles[i][j].x = i*((float)swidth/(float)dist_x);
     particles[i][j].y = j*((float)sheight/(float)dist_y);
     particles[i][j].vx = vx;
     particles[i][j].vy = vy;
    }
  }
}

void draw() {
  
  for (int i = 0; i < dist_x; i++) {
    for (int j = 0; j < dist_y; j++) {
      
      particles[i][j].x += particles[i][j].vx;
      particles[i][j].y += particles[i][j].vy;
 
    }
  }  
  
  background(255);
  for (int i = 0; i < dist_x; i++) {
    for (int j = 0; j < dist_y; j++) {
      point(particles[i][j].x, particles[i][j].y);
    }
  }
  
}

class Particle {
 public float x;
 public float y;
 public float vx;
 public float vy; 
}

