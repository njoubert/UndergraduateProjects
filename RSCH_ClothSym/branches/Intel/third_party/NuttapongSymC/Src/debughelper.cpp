#include "debughelper.h"

/*
extern void WriteMFile(vector<real>& vec, string name) {
	FILE* f = fopen(name.c_str(), "wt");
	int dim = vec.size();
	for (int i = 0; i < dim; i++) {
		fprintf(f, "%lg\n", vec[i]);
	}
	fclose(f);
}

extern void WriteMFile(Vector<real>& vec, string name) {
	FILE* f = fopen(name.c_str(), "wt");
	int dim = vec.size();
	for (int i = 0; i < dim; i++) {
		fprintf(f, "%lg\n", vec[i]);
	}
	fclose(f);
}

extern void WriteMFile(CCoorMatrix<real>& mat, string name) {
	int nnz = mat.nnz();
	FILE* f = fopen(name.c_str(), "wt");
	
	fprintf(f,"%d %d %d\n", mat.nrows(), mat.ncols(), mat.nnz());
	const unsigned *ip = mat.getI();
	const unsigned *jp = mat.getJ();
	real* vp = mat.getA();
	for (int i = 0; i < nnz; i++) {
		//fprintf(f, "%d %d %lf\n", mat.row_ind(i)+1, mat.col_ind(i)+1, mat.val(i));	
   #ifdef USE_DOUBLE
	   fprintf(f, "%d %d %lg\n", ip[i]+1, jp[i]+1, vp[i]);
   #else
	   fprintf(f, "%d %d %g\n", ip[i]+1, jp[i]+1, vp[i]);
   #endif
	}
	fclose(f);
}*/