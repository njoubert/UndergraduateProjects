import peasy.org.apache.commons.math.*;
import peasy.*;
import peasy.org.apache.commons.math.geometry.*;

PeasyCam cam;


class DataLine {
  int i;
  float rox,roy,roz,rdx,rdy,rdz;
  float nx, ny, nz;
  float hitx, hity, hitz;
  
}

class TracedRay {
  DataLine[] lines = new DataLine[20];
  int linescount = 0;
}

class LensSurface {
  float radius;
  float axpos;
  float N;
  float aperture;
}
  

BufferedReader reader;
BufferedReader readerLenses;
String line;
String specfile;
TracedRay[] rays = new TracedRay[32000];
int rayscount = 0;
LensSurface[] lenses = new LensSurface[30];
int lensescount = 0;

boolean showNormals = true;
boolean showRayDirections = true;
boolean showRayTraces = true;

void keyPressed() {
  if (key == 'n')  //n for normal
    showNormals = !showNormals;
  if (key == 't')  //t for traces
    showRayTraces = !showRayTraces;
  if (key == 'd')  //d for directions
    showRayDirections = !showRayDirections;
}

float _film_width, _film_height, _film_distance, _lens_thickness;

void setup() {
  size(800,800,P3D);
  
  cam = new PeasyCam(this, 100);
    reader = createReader("/Users/njoubert/Dropbox/CODE/pbrt-1.04/hw3/viz.txt"); 
    
    try {
    line = reader.readLine();
    String[] pieces = split(line, TAB);
    specfile = pieces[0];
    _film_width = float(pieces[1]);
    _film_height = float(pieces[2]);
    _film_distance = float(pieces[3]);
    _lens_thickness = float(pieces[4]);
    
    System.out.println(specfile);
    TracedRay tr = new TracedRay();
    while(reader.ready()) {
      line = reader.readLine();
      if (line.charAt(0) == '=') {
        rays[rayscount] = tr;
        rayscount++;
        tr = new TracedRay();
      } else {
        
      pieces = split(line, TAB);
      DataLine dline = new DataLine();
      dline.i = int(pieces[0]);
      dline.rox = float(pieces[1]);
      dline.roy = float(pieces[2]);
      dline.roz = float(pieces[3]);
      dline.rdx = float(pieces[4]);
      dline.rdy = float(pieces[5]);
      dline.rdz = float(pieces[6]);
      dline.nx = float(pieces[7]);
      dline.ny = float(pieces[8]);
      dline.nz = float(pieces[9]);
      dline.hitx = float(pieces[10]);
      dline.hity = float(pieces[11]);
      dline.hitz = float(pieces[12]);
      tr.lines[tr.linescount] = dline;
      tr.linescount += 1;
      }
      
    }
    
    readerLenses = createReader("/Users/njoubert/Dropbox/CODE/pbrt-1.04/hw3/" + specfile);
    LensSurface temp[] = new LensSurface[30];
    
    while (readerLenses.ready()) {
       line = readerLenses.readLine();
       if (line.charAt(0) != '#') {
        pieces = split(line, TAB);
        LensSurface ls = new LensSurface();
        ls.radius = float(pieces[0]);
        ls.axpos = float(pieces[1]);
        ls.N = float(pieces[2]);
        ls.aperture = float(pieces[3]);
        temp[lensescount] = ls;
        lensescount += 1; 
       }
    }
    int count = 0;
    for (int i = lensescount-1; i >= 0; i--) {
      lenses[count] = temp[i];
      count++;
    }
    
    
    
    } catch (IOException e) {
      e.printStackTrace();
      exit();
    }
}

void drawFilm() {
  pushMatrix(); 
  translate(0,0,-_film_distance - _lens_thickness);
  fill(0,0,0,128);
  rect(-_film_width/2, -_film_height/2, _film_width, _film_height);
  rotateY(PI);
  rect(-_film_width/2, -_film_height/2, _film_width, _film_height);
  
  popMatrix();
}

void drawLenses() {
 
 float zoffset = -_lens_thickness;
 for (int i = 0; i < lensescount; i++) {
  LensSurface ls = lenses[i];
  zoffset += ls.axpos;
  
  fill(104,102,0);
  pushMatrix();
  translate(0,0,zoffset);
  arc(0,0,ls.aperture,ls.aperture,0,TWO_PI);
  popMatrix();
  
  noFill();

 } 
}

void drawTracedRay(TracedRay tr) {
  strokeWeight(1);
  for (int i = 0; i < tr.linescount; i++) {
    DataLine dl = tr.lines[i];
    
    stroke(0,0,0);
    strokeWeight(2);
    point(dl.rox, dl.roy, dl.roz);
    strokeWeight(1);
    stroke(0,0,255);
    if (showRayTraces)
      line(dl.rox, dl.roy, dl.roz, dl.hitx, dl.hity, dl.hitz);
    stroke(0,255,0);
    if (showNormals)
      line(dl.hitx, dl.hity, dl.hitz, dl.hitx + dl.nx, dl.hity + dl.ny, dl.hitz + dl.nz);    
    stroke(255,0,0);
    if (showRayDirections)
      line(dl.rox, dl.roy, dl.roz, dl.rox + dl.rdx, dl.roy + dl.rdy, dl.roz + dl.rdz);
  }
}

void draw() {
  background(255);
//  translate(900, 400, 0);
//  rotateY(PI/2.0);
//  scale(6.0);
    
  stroke(0,0,0);
  strokeWeight(1);
  line(0,0,-200,0,0,200);
  
  for (int i = 0; i < rayscount; i++) {
    //if (abs(rays[i].lines[0].rox) < 2.0 && abs(rays[i].lines[0].roy) < 2.0)
      drawTracedRay(rays[i]);
  }
  
  drawFilm();
  drawLenses();
  
}
