

void setup() {
  size(500,500);
}

void drawJointAndBone(int l, int radii, int rotation) {
  fill(255);
  stroke(0);

  pushMatrix();
  rotate(radians(rotation));
  ellipse(0,0,2*radii,2*radii);
  quad(0, radii, l, 0, 0, -radii, 0, radii);
  line(-radii,0,l,0);
  popMatrix();
  
  stroke(255,0,0);
  
  if (rotation > 180) {
    arc(0, 0, 50, 50, radians(180 - rotation), 0);
  } else if (rotation < 0) {
    arc(0, 0, 50, 50, radians(rotation), 0);
  } else {
    arc(0, 0, 50, 50, 0, radians(rotation));
  }
  line(0,0, 80, 0);
  rotate(radians(rotation));
  
  translate(l, 0);
}

void draw() {
  strokeWeight(1.0);
  background(255);
  translate(60, 300);
  drawJointAndBone(150, 20, -25);
  drawJointAndBone(150, 20, 25);
  drawJointAndBone(150, 20, 25);
}
