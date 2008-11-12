
//class Position3d {
//public:
//	Position3d() { inputx = 0; inputy = 0; inputz = 0; }
//	float getX() { return inputx; }
//	float getY() { return inputy; }
//	float getZ() { return inputz; }
//	void setX(float x) { this->inputx = x; }
//	void setY(float y) { this->inputy = y; }
//	void setZ(float z) { this->inputz = z; }	
//	void setPos(float x, float y, float z) {
//		setX(x); setY(y); setZ(z);
//	}
//	void debugMe() { printDebug("(" << inputx << "," << inputy << "," << inputz << ")"); }
//private:	
//	float inputx, inputy, inputz; // width and height
//};



//class Light {
//public:
//	Light() {
//		printDebug("Creating a light...");
//	}
//	//This creates a light with position (x,y,z) and color (r,g,b)
//	//x, y, z are relative to sphere's center (0 is center, 1 is radius)
//	//r, g, b are positive floats between [0.0, max_float]
//	Light(float x, float y, float z, float r, float g, float b) {
//		printDebug("Creating Light at " << "(" << x << "," << y << "," << z << ") with color (" << r << "," << g << "," << b << ");");
//		setPosition(x,y,z);
//		setLuminosity(r,g,b);
//	}
//	void setPosition(float x, float y, float z) {
//		P_relPosition.setX(x);
//		P_relPosition.setY(y);
//		P_relPosition.setZ(z);
//	}
//	Position3d * getPosition() {
//		return &P_relPosition;
//	}
//	
//	void setLuminosity(float r, float g, float b) {
//		C_lum.setColor(r,g,b);	
//	}
//	
//	virtual Vector3d getLightingVector(Position3d pos) = 0;
//	
//	Color * getLuminosity() {
//		return &C_lum;
//	}
//private:
//	Position3d P_relPosition;
//	Color C_lum;
//};
//
//class PointLight: public Light {
//public:
//	PointLight(float x, float y, float z, float r, float g, float b):Light(x,y,z,r,g,b){}
//	Vector3d getLightingVector(Position3d pos) {
//		Vector3d temp;
//		return temp;
//	}
//};
//
//class DirectionalLight: public Light {
//public:
//	DirectionalLight(float x, float y, float z, float r, float g, float b):Light(x,y,z,r,g,b){}	
//	Vector3d getLightingVector(Position3d pos) {
//		Vector3d temp;
//		return temp;
//	}
//};



class Object3d {
public:
	Object3d() : C_ambient(0,0,0), C_diffuse(0,0,0), C_specular(0,0,0) {}
	virtual ~Object3d() {}
	void setAmbientColor(float r, float g, float b) {
		//Assume r g b values are [0.0,1.0]
		printDebug("Object3d::setAmbientColor(" << r << "," << g << "," << b << ");");
		C_ambient.setColor(r,g,b);
	}
	void setDiffuseColor(float r, float g, float b) {
		//Assume r g b values are [0.0,1.0]
		printDebug("Object3d.setDiffuseColor(" << r << "," << g << "," << b << ");");
		C_diffuse.setColor(r,g,b);
	}
	void setSpecularColor(float r, float g, float b) {
		//Assume r g b values are [0.0,1.0]
		printDebug("Object3d.setSpecularColor(" << r << "," << g << "," << b << ");");
		C_specular.setColor(r,g,b);
	}
	void setSpecularCoeff(float arg) {
		printDebug("Object3d.setSpecular(" << arg << ");");
		v = arg;
	}
	virtual void render(vector<Light*> & lights, Vector3d & view) = 0;
protected:
	float v;
	Color C_ambient;
	Color C_diffuse;
	Color C_specular;
};

class Sphere: public Object3d {
public:
	Sphere() {
		radius = 100;
		center.setPos(0,0,0);
	}
	~Sphere() {}
	
	void setRadiusfromViewport(Viewport & v) {
		radius = (int) (0.4 * min(v.w, v.h));
	}
	
	void setRadius(float x) {
		radius = x;
	}
	
	int getRadius() {
		return (int) radius;
	}
	
	void render(vector<Light*> & lights, Vector3d & view) {
		Position3d point;
		Vector3d normal;
		Vector3d normalcp;
		Vector3d incidence;
		Vector3d reflectance;
		Color C_pixel;
		Color C_tempDiff;
		Color C_tempSpec;
		Light* l;
		
		printDebug("Shading a sphere...");
		
		glBegin(GL_POINTS);
			
		for (float i = -radius; i <= radius; i += 1) {
			float width = (sqrt((float)(radius*radius-i*i)) - 0.5f);
			for (float j = -width; j <= width; j += 1) {
				
		/*

1) compute the xyz location of the point on the sphere.  Since you know the xy location of the pixel and radius of the sphere, you can figure out the z coordinate.  Call this location "p".  Compute the normal for the point also, "n".

2) if the pixel is outside the radius of the sphere set it black on move on

3) set the initial color for the pixel to ka

4) for each light

	4a) compute l = l_pos_i-p
	4b) v = [0,0,1]
	4c) r = reflect(v,n) (see text)
	4c) c = kd * l_i * ( n . l ) + ks * l_i * (r.l)^q
	4d) pixel color += c

5) draw pixel
				
*/				
				C_pixel = C_ambient;
				
				point.setPos(center.getX() + i, center.getY() + j,0);
				calculateZ(point);
				
				normal.calculateFromPositions(&center,&point);
				normal = normal.normalize(radius);
				
				for (unsigned int i = 0; i < lights.size(); i++) {
						
					l = lights[i];
					incidence.calculateFromPositions(l->getPosition(),&point);
					incidence.normalize();

					reflectance.calculateReflective(&incidence, &normal);
					
					C_tempDiff = C_diffuse * ((l->getLuminosity()));
					C_tempDiff *= max(0.0f, incidence.dot(&normal));
					
					C_tempSpec = C_specular * ((l->getLuminosity()));
					C_tempSpec *= pow(max(0.0f, view.dot(&reflectance)), v);
					
					C_pixel += C_tempDiff;
					C_pixel += C_tempSpec;
					
				}
				
				C_pixel.normalizeTo(C_pixel.getMaxVal());
				setPixel(point.getX(), point.getY(), C_pixel.getNormalizedR(1), C_pixel.getNormalizedG(1), C_pixel.getNormalizedB(1));
				
				
			}
		}
		
		glEnd();
	}
private:
	float radius;
	Position3d center;
	
	//Uses the x and y coordinates to calculate z on a sphere.
	void calculateZ(Position3d & pos) {
		//Use pythoagoras' theorem in 3D:
		float x = pos.getX();
		float y = pos.getY();
		pos.setZ(sqrt(radius*radius - x*x - y*y));
	}
	
	void setPixel(float x, float y, GLfloat r, GLfloat g, GLfloat b) {
		glColor3f(r, g, b);
		glVertex2f(x, y);
	}
	
};