#include "main.h"

using namespace std;

void loadObjects(VCollide * vc) {
	vc.NewObject(&id[0]);
	
	v1[0] = 1.0; v1[1] = 0.0; v1[2] = 0.0;
	v2[0] = 0.0; v2[1] = 0.0; v2[2] = 0.0;
	v3[0] = 0.0; v3[1] = 1.0; v3[2] = 0.0;
	vc.AddTri(v1, v2, v3, 0);
	
	vc.EndObject();
}

int main(int argc, char *argv[]) {
  	VCollide vc;
	int id[2] = {0, 1};

	loadObjects(&vc);
	
	
	VCReport report;
	
	vc.Collide( &report, VC_ALL_CONTACTS);
	
	int j;
	for (j = 0; j < report.numObjPairs(); j++)
		cout<<"Detected collision between objects "<<report.obj1ID(j) <<" and "<< report.obj2ID(j) <<"\n";
	
}

	
}