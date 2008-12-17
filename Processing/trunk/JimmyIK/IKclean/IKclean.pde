import Jama.util.*;
import Jama.*;

Handle[] handles; // control forward kinematics
Handle[] errorHandles; // control SVD 0-or-not threshold
float[] values; // angles at joints (in degrees)
float[] lengths; // lengths of limbs
float[] px; // x coordinates of joints
float[] py; // y coordinates of joints
color[] colors; // colors of joints
int num; // number of joints
PFont fontA; // display font

float goalx = 10, goaly = 60; // position of target for ik
boolean goalOver = false, goalPress = false; // remember if the user was over or clicking the goal (to make it draggable)
float startx = 2*width/5; // x position of the root joint
float starty = 3*height/8; // y position of the root joint
boolean firstIter = true; // hack to let me skip over the J^-P computation on the first iteration (since J is calculated after J^-P in the loop)
int mode; // the mode of the program, value from 1 to 4
//PrintWriter output; // used for debugging

Matrix J; // the jacobian

// set up default values
void setup()
{
  //output = createWriter("debug.txt");
  size(500,500);
  num = 3;
  handles = new Handle[num];
  values = new float[3];
  lengths = new float[3];
  colors = new color[3];
  px = new float[3];
  py = new float[3];
  lengths[0] = 40; lengths[1] = 60; lengths[2] = 30;
  colors[0] =  color(255,0,0);
  colors[1] =  color(0,255,0);
  colors[2] =  color(0,0,255);
  values[0] = 25; values[1] = 10; values[2] = 0;
  startx = 2*width/5;
  starty = 3*height/8;
  mode = 1;
  firstIter = true;
  for (int i=0; i<num; i++)
  {
    handles[i] = new Handle(4*width/5, 10+i*15, convertFromAngle(values[i]), 10, handles, i);
  }
  errorHandles = new Handle[1];
  errorHandles[0] = new Handle(4*width/5, height-105, 50, 10, errorHandles, -1);
  fontA = createFont("FFScala", 15);
  textFont(fontA, 15);
  
  double[][] M = new double[2][num];
  J = new Matrix(2, num);
}

// true if the goal is being dragged about
boolean goalLocked = false;
// true if we had a key pressed last frame (lets me only respond to a key press once)
boolean lastPressed = false;

// draws dotted lines
void dottedLine(float a, float b, float c, float d)
{
  for (float u = 0; u < 1; u += .02)
  {
    float v = u;
    float xa = v * c + (1-v) * a;
    float ya = v * d + (1-v) * b;
    v = u+.01;
    float xb = v * c + (1-v) * a;
    float yb = v * d + (1-v) * b;
    line(xa,ya,xb,yb);
  }
}

// for debug
/*void PrintMatrix(String name, Matrix m)
{
  output.println(name + ": ");
  for (int i = 0; i < m.getRowDimension(); i++)
  {
    for (int j = 0; j < m.getColumnDimension(); j++)
    {
      output.print(m.get(i,j) + " ");
    }
    output.println();
  }
}*/

void draw()
{
  background(153);
  
  // set mode
  if (keyPressed) {
    if (key == '1')
      mode = 1;
    if (key == '2')
      mode = 2;
    if ( key == '3')
      mode = 3;
     if (key == '4')
       mode = 4;
  }
  
  // update handles with user input (for forward ik)
  for (int i=0; i<num; i++) {
    handles[i].update();
    stroke(colors[i]);
    handles[i].display(colors[i]);
  }
  if (mode >= 4) { // for error threshold
    errorHandles[0].update();
    errorHandles[0].display(color(255));
  }
  
  // black rectangle covers the left part of the screen
  fill(0);
  rect(0,0,4*width/5,height);
  
  // update the error threshold according to the handle's input
  float errorThresh = convertToAngle(errorHandles[0].length) / 360.0f;
  errorThresh = .1 * (1-errorThresh) + .00001 * (errorThresh);
  if (mode >= 4) { // if we're in the right mode, display threshold value
    textAlign(RIGHT);
    fill(255);
    text("Threshold = " + errorThresh, 4*width/5,height-100);
  }
  
  // draw instructions
  textAlign(LEFT);
  fill(255);
  text("Press 1 through 4 to change mode (currently " + mode + ")",3, height-30);
  if (mode >= 3)
    text("Press s to step maximal amount; c to step conservatively", 3, height-15);
  
  // handle input to move the goal about
  goalOver = overCirc(goalx, goaly, 10);
  if (goalOver && mousePressed || goalLocked) {
    goalPress = true;
    goalx = mouseX;
    goaly = mouseY;
    goalLocked = true;
  }
  else
  {
    goalPress = false;
    goalLocked = false;
  }
  
  // draw the goal
  if (mode >= 3) {
    fill(255,255,0);
    ellipse(goalx, goaly, 10, 10);
    if (goalOver || goalPress)
    {
      fill(100,0,0);
      ellipse(goalx, goaly, 5, 5);
      fill(255,0,255);
      text("goal", goalx+10, goaly+5);
    }
    else {
        fill(255);
      text("goal", goalx+10, goaly+5);
    }
  }
  
  // iterate through the chain of joints and draw them
  float xpos = startx;
  float ypos = starty;
  
  // starting joint is white
  fill(255);
  ellipse(xpos, ypos, 10, 10);
  float angle = 0;
  
  fill(255);
  for (int i = 0; i < num; i++)
  {
    fill(colors[i]);
    textAlign(RIGHT);
    angle += values[i];
    text(values[i], 4*width/5, 10+i*15+5); // write the angle value next to the appropriate handle
    float lastx = xpos;
    float lasty = ypos;
    px[i] = xpos;
    py[i] = ypos;
    xpos += lengths[i] * cos(radians(angle));
    ypos += lengths[i] * sin(radians(angle));
    stroke(255);
    line(lastx, lasty, xpos, ypos);
    ellipse(xpos, ypos, 10, 10);
  }
  
  // compute J^-P
  // J is computed later in the function, so skip this on the firstIter to avoid computing J^-P before we know J
  if (!firstIter && mode >= 3) { 
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
      
      // create a matrix pointing from the end effector to the goal
      Matrix deltaP = new Matrix(2, 1);
      deltaP.set(0, 0, goalx - xpos);
      deltaP.set(1, 0, goaly - ypos);
  
      // solve for c, the needed change in angle
      Matrix c = pseudoInv.times(deltaP);

      // write out the solution in interactive format
      fill(255);
      textAlign(LEFT);
      text("J^-P dp = [ ", 10, height - 130);
      text("] ^T", 290, height-130);
      float accumAng = 0;
      for (int i = 0; i < num; i++)
      {
        fill(colors[i]);
        text((float)c.get(i,0), 80+70*i, height-130);
        float dx = px[i] - xpos;
        float dy = py[i] - ypos;
        float distance = sqrt(dx*dx+dy*dy)*2;

        float offIs = radians((float)c.get(i,0));
        float startIs = radians(accumAng + values[i]);
        accumAng += values[i];
        if (offIs < 0)
        {
          offIs *= -1;
          startIs -= offIs;
        }
        if (overRect(78+i*70, height - 145, 70, 18) || errorHandles[0].press || handles[i].press)
        {
          fill(0,0);
          stroke(255);
          rect(78+i*70, height - 145, 70, 18);
          stroke(colors[i]);
          arc(px[i], py[i], lengths[i]*2, lengths[i]*2, startIs, startIs+offIs);
        }
      }
      
      // draw the effect of the solution in dotted line format
      float effectx = xpos;
      float effecty = ypos;
      for (int i = 0; i < num; i++)
      {
        Matrix c_Temp = new Matrix(num, 1);
        c_Temp.set(i,0,c.get(i,0));
        Matrix doutTemp = J.times(c_Temp);
        float lastx = effectx;
        float lasty = effecty;
        effectx += (float)doutTemp.get(0,0);
        effecty += (float)doutTemp.get(1,0);
        stroke(colors[i]);
        dottedLine(lastx, lasty, effectx, effecty);
      }
      
      // respond to keypresses only once
      if (!keyPressed)
        lastPressed = false;
      if (keyPressed && !lastPressed) {
        lastPressed = true;
        
        // decide if we're going to take a step
        // and if so, if it's conservative or not
        boolean conserve = false;
        boolean step = false;
        if (key == 'c' || key == 'C') {
          conserve = true;
          step = true;
        }
        if (key == 's' || key == 'S') {
          step = true;
        }
        
        if (step) {
          // compute the current error
          float dx = (goalx - xpos);
          float dy = (goaly - ypos);
          float curErr = dx * dx + dy * dy;
          float error = 0;
          // offs arrays store the changes in angle we're thinking about taking
          float offs[] = new float[num];
          for (int i = 0; i < num; i++) {
            offs[i] = (float)c.get(i, 0);
            offs[i] = (float)c.get(i, 0);
          }
          
          // for a conservative step, we'll do a binary search
          if (conserve) {
            int count = 0; // count the number of search steps so we can give up after a fixed number of steps
            do {
              count++;
              
              // compute what the new position would be if we took this step
              float newx = computeEndX(offs);
              float newy = computeEndY(offs);
              // and what the associated error would be
              dx = goalx - newx;
              dy = goaly - newy;
              float newErr = dx*dx + dy*dy;
              // if we've improved, take the step
              if (newErr < curErr)
              {
                // add in the offsets, store the angles in the range [0,360]
                for (int i = 0; i < num; i++)
                {
                  float newAngle = values[i] + offs[i];
                  //output.println("adding: " + values[i] + " + " + offs[i]);
                  while (newAngle > 360) {
                    newAngle -= 360;
                  }
                  while (newAngle < 0) {
                    newAngle += 360;
                  }
                  values[i] = newAngle;
                }
                break;
              }
              // if we haven't, cut step size and try again
              for (int i = 0; i < num; i++)
              {
                offs[i] *= .5;
              }
            } while (count < 30); // just give up after 30 steps
          }
          else
          {
            // if not conservative, just take the full step with no search
            // add in the offsets, store the angles in the range [0,360]
            for (int i = 0; i < num; i++)
            {
              float newAngle = values[i] + offs[i];
              while (newAngle > 360) {
                newAngle -= 360;
              }
              while (newAngle < 0) {
                newAngle += 360;
              }
              values[i] = newAngle;
            }
          }
        }
      }
  }
  firstIter = false;
  
  // load the computed angles in to the handles, so the gui remains consistent with the state
  for (int i = 0; i < num; i++)
  {
    handles[i].SetLength(convertFromAngle(values[i]));
  }
  
  // draw the frame for the J matrix
  if (mode >= 2) {
    fill(255);
    stroke(255);
    textAlign(LEFT);
    text("J = ", 10, 300+20);
    line(34, 290, 34, 340);
    line(34, 290, 44, 290);
    line(34, 340, 44, 340);
    line(230, 290, 230, 340);
    line(230, 340, 220, 340);
    line(230, 290, 220, 290);
  }
  
  // compute J
  // i and j are backwards here ... oops
  for (int i = 0; i < J.getColumnDimension(); i++)
  {
    // use finite differences to compute, because I'm lazy
    float endOffX = computeEndX(i, 1);
    J.set(0,i,endOffX - xpos);
    float endOffY = computeEndY(i, 1);
    J.set(1,i,endOffY - ypos);
    
    // make the J matrix interactive
    if (overRect(37+i*70, 295, 50, 38) || handles[i].press)
    {
      if (mode >= 2) {
        fill(15,15);
        rect(37+i*70, 295, 50, 38);
        stroke(colors[i]);
        line(xpos, ypos, xpos + (endOffX - xpos) * 30, 
                        ypos + (endOffY - ypos) * 30);
        float dx = px[i] - xpos;
        float dy = py[i] - ypos;
        float distance = sqrt(dx*dx+dy*dy)*2;
        stroke(200,100);
        ellipse(px[i], py[i], distance, distance);
      }
    }
    if (mode >= 2) { // write the values for the J matrix
      fill(colors[i]);
      for (int j = 0; j < J.getRowDimension(); j++)
      {
        text((float)J.get(j,i), 40+i*70, 300+10+j*20);
      }
    }
  }  
}

// compute the x position of the end effector if the ith joint moves by off degrees
float computeEndX(int i_delta, float off)
{
  float angle = 0;
  float xpos = startx;
  for (int i = 0; i < num; i++)
  {
    if (i == i_delta)
      angle += off;
    angle += values[i];
    xpos += lengths[i] * cos(radians(angle));
  }
  return xpos;
}

// compute the y position of the end effector if the ith joint moves by off degrees
float computeEndY(int i_delta, float off)
{
  float angle = 0;
  float ypos = starty;
  for (int i = 0; i < num; i++)
  {
    if (i == i_delta)
      angle += off;
    angle += values[i];
    ypos += lengths[i] * sin(radians(angle));
  }
  return ypos;
}

// compute the x position of the end effector if the joints are offset by the values in the off vector
float computeEndX(float[] off)
{
  float angle = 0;
  float xpos = startx;
  for (int i = 0; i < num; i++)
  {
    angle += off[i];
    angle += values[i];
    xpos += lengths[i] * cos(radians(angle));
  }
  return xpos;
}

// compute the y position of the end effector if the joints are offset by the values in the off vector
float computeEndY(float[] off)
{
  float angle = 0;
  float ypos = starty;
  for (int i = 0; i < num; i++)
  {
    angle += off[i];
    angle += values[i];
    ypos += lengths[i] * sin(radians(angle));
  }
  return ypos;
}

// 2 functions used to convert from handle length to the 0-360 range, and back again
float convertToAngle(int len)
{
  return len * 360 / (float)(width/5 - 10 - 1);
}
int convertFromAngle(float ang)
{
  return (int) ( ang * (width/5 - 10 - 1) / 360 );
}

void mouseReleased()
{
  for (int i=0; i<num; i++) {
    handles[i].release();
  }
  errorHandles[0].release();
  goalLocked = false;
}

class Handle
{
  int number;
  int x, y;
  int boxx, boxy;
  int length;
  int size;
  boolean over;
  boolean press;
  boolean locked = false;
  boolean otherslocked = false;
  Handle[] others;
  
  void SetLength(int len)
  {
    length = len;
  }
  
  Handle(int ix, int iy, int il, int is, Handle[] o, int inumber)
  {
    x = ix;
    y = iy;
    length = il;
    size = is;
    boxx = x+length - size/2;
    boxy = y - size/2;
    others = o;
    number = inumber;
  }
  
  void update() 
  {
    boxx = x+length;
    boxy = y - size/2;
    
    for(int i=0; i<others.length; i++) {
      if(others[i].locked == true) {
        otherslocked = true;
        break;
      } else {
        otherslocked = false;
      }  
    }
    
    if(otherslocked == false) {
      over();
      press();
    }
    
    if(press) {
      length = lock(mouseX-width*4/5-size/2, 0, width/5-size-1);
      if (number != -1)
        values[number] = convertToAngle(length);
    }
  }
  
  void over()
  {
    if(overRect(boxx, boxy, size, size)) {
      over = true;
    } else {
      over = false;
    }
  }
  
  void press()
  {
    if(over && mousePressed || locked) {
      press = true;
      locked = true;
    } else {
      press = false;
    }
  }
  
  void release()
  {
    locked = false;
  }
  
  void display(color c) 
  {
    boxx = x+length;
    boxy = y - size/2;
    line(x, y, x+length, y);
    fill(c);
    stroke(0);
    rect(boxx, boxy, size, size);
    if(over || press) {
      line(boxx, boxy, boxx+size, boxy+size);
      line(boxx, boxy+size, boxx+size, boxy);
    }

  }
}

boolean overRect(int x, int y, int width, int height) 
{
  if (mouseX >= x && mouseX <= x+width && 
      mouseY >= y && mouseY <= y+height) {
    return true;
  } else {
    return false;
  }
}

boolean overCirc(float x, float y, float rad)
{
  float dx = x - mouseX;
  float dy = y - mouseY;
  return (dx*dx + dy*dy < rad*rad);
}

int lock(int val, int minv, int maxv) 
{ 
  return  min(max(val, minv), maxv); 
} 
