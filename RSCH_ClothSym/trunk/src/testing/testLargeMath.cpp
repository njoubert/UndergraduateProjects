/*
 * testLargeMath.cpp
 *
 *  Created on: Oct 31, 2008
 *      Author: njoubert
 */

#include <iostream>

#include "testing.h"
#include "../math/algebra3.h"
#include "../math/LargeVector.h"
#include "../math/LargeMatrix.h"
#include "../util/Timer.h"

#include "../math/CG.h"

#include <omp.h>

using namespace std;

void testVector() {
    printHeading("TESTING DIFFERENT TYPES ASSIGNMENTS");

    LargeVec3Vector v2(5);
    v2[0] = vec3(0, 1, 2);
    v2[1][0] = 3;
    v2[1][1] = 4;
    v2[1][2] = 5;
    cout << v2[0] << endl;
    cout << v2[1] << endl;

    printHeading("TESTING OPERATORS:");

    LargeVec3Vector a1(5);
    LargeVec3Vector a2(5);
    a1[0] = a1[1] = a1[2] = a1[3] = vec3(2, 3, 4);
    a2[0] = a2[1] = a2[2] = a2[3] = vec3(3, 4, 5);
    cout << "a1 = " << a1 << endl;
    cout << "a2 = " << a2 << endl;
    a1 += a2;
    cout << "a1 += a2;" << endl;
    cout << "a1 = " << a1 << endl;

    LargeVec3Vector b1(4);
    LargeVec3Vector b2(4);
    for (int i = 0; i < 4; i++) {
        b1[i] = vec3(rand() % 100, rand() % 100, rand() % 100);
        b2[i] = vec3(i, i * 10, i * 100);
    }
    cout << "b1 = " << b1 << endl;
    cout << "b2 = " << b2 << endl;
    cout << "b1 += b2" << endl;
    b1 += b2;
    cout << "b1 = " << b1 << endl;

    printHeading("TESTING COPY CONSTRUCTOR");
    LargeVec3Vector c1(b1);

    cout << "c1(b1)" << endl;
    cout << "c1 = " << c1 << endl;
    b1[1] = vec3(0, 0, 0);
    b1[2] = vec3(1, 1, 1);
    b1[3] = vec3(2, 2, 2);
    cout << "b1 = " << b1 << endl;
    cout << "c1 = " << c1 << endl;

    LargeVec3Vector * vp = new LargeVec3Vector(b1);

    cout << "v  = " << *vp << endl;
    delete vp;

    printHeading("TESTING POINTERS");
    vec3* p = &b1[1];
    cout << "b1 = " << b1 << endl;
    (*p)[0] = 100000;
    (*p)[1] = 100000;
    cout << "b1 = " << b1 << endl;

}

void testMatrix() {
    LargeMat3Matrix v(5, 5);

    cout << "v(5,5)" << endl;
    cout << "This should not be in the sparse row:" << endl;
    cout << "v(0,0) nonzero? " << v.isNonZero(0, 0) << endl;

    printHeading("Test sparsity pattern");
    vector<vector<int> > pattern(10);
    for (int i = 0; i < 10; i++) {
        pattern[i].push_back(i);
    }

    cout << "LargeMat3Matrix s(10,10,pattern);" << endl;
    LargeMat3Matrix s(10, 10, pattern, true);
    printResult(s);

    cout << "Testing retrieval of elements:" << endl;
    cout << "s(1,1) = " << endl;
    printResult(s(1,1) << endl);

}

void testMatrixVectorMultiply() {
    vector<vector<int> > pattern(10);
    for (int i = 0; i < 10; i++) {
        pattern[i].push_back(i);
    }

    LargeMat3Matrix s(10, 10, pattern, true);
    LargeVec3Vector in(10, 5);
    LargeVec3Vector out(10);
    cout << "[10,10] Identity postMultiply(vector of all 5):" << endl;
    s.postMultiply(in, out);
    cout << " = " << out << endl;

    //-------------------------------------
    LargeVec3Vector inn(10, 6);
    cout << "[10,10] Identity preMultiply(vector of all 6):" << endl;
    s.preMultiply(inn, out, true);
    cout << " = " << out << endl;

    //-------------------------------------


    cout << "Testing multiply with banded blocks of incrementing numbers."
            << endl;
    LargeMat3Matrix s2(10, 10, pattern, true);
    LargeVec3Vector in2(10, 1);
    cout << "s2 = " << endl << s2 << endl;
    cout << "s2(0,0) = 0" << endl;
    s2(0, 0) = mat3(0);
    cout << "s2(1,1) = 1" << endl;
    s2(1, 1) = mat3(1);
    cout << "s2(2,2) = 2" << endl;
    s2(2, 2) = mat3(2);
    cout << "s2(3,3) = 3" << endl;
    s2(3, 3) = mat3(3);
    cout << "s2(4,4) = 4" << endl;
    s2(4, 4) = mat3(4);
    cout << "s2(5,5) = 5" << endl;
    s2(5, 5) = mat3(5);
    cout << "s2(6,6) = 6" << endl;
    s2(6, 6) = mat3(6);
    cout << "s2(7,7) = 7" << endl;
    s2(7, 7) = mat3(7);
    cout << "s2(8,8) = 8" << endl;
    s2(8, 8) = mat3(8);
    cout << "s2(9,9) = 9" << endl;
    s2(9, 9) = mat3(9);

    cout << "[10,10] banded postMultiply(vector of all 1):" << endl;
    s2.postMultiply(in2, out);
    cout << " = " << out << endl;

    //-------------------------------------

    LargeVec3Vector in2n(10, 2);
    cout << "[10,10] banded preMultiply(vector of all 2):" << endl;
    s2.preMultiply(in2n, out, true);
    cout << " = " << out << endl;

    //-------------------------------------

    //Now to check some not-very-arbitrary sparse matrices...
    cout << "Creating a not-very-arbitrary sparse matrix pattern." << endl;
    vector<vector<int> > sp(6);
    sp[0].push_back(0);
    sp[0].push_back(2);
    sp[1].push_back(1);
    sp[1].push_back(3);
    sp[2].push_back(0);
    sp[2].push_back(2);
    sp[3].push_back(1);
    sp[3].push_back(3);
    LargeMat3Matrix M1(6, 6, sp, true);
    cout << M1;
    cout << "Creating matrix." << endl;
    M1(0, 0) = mat3(120);
    M1(0, 2) = mat3(33);
    M1(1, 1) = mat3(150);
    M1(1, 3) = mat3(66);
    M1(2, 0) = mat3(190);
    M1(2, 2) = mat3(99);
    M1(3, 1) = mat3(210);
    M1(3, 3) = mat3(122);
    cout << M1 << endl;

    M1.outAsMatlab(cout);
    cout << endl;

    cout << "Creating a not-very-random vector." << endl;
    LargeVec3Vector in3(6);
    in3[0] = vec3(1, 2, 3);
    in3[1] = vec3(4, 5, 6);
    in3[2] = vec3(7, 8, 9);
    in3[3] = vec3(-1, -6, -9);
    in3[4] = vec3(-3, -7, 39);
    in3[5] = vec3(-99, 22, 33);
    cout << "in = " << in3 << endl;

    LargeVec3Vector out3(6);

    //Test postmultiply:
    cout << "Test Postmultiply:" << endl;
    cout << "COMPUTED: " << M1.postMultiply(in3, out3) << endl;
    cout
            << "EXPECTED: (| 1512 1512 1512 |,| 1194 1194 1194 |,| 3516 3516 3516 |,| 1198 1198 1198 |,| 0 0 0 |,| 0 0 0 |)"
            << endl;
    //i love matlab


    //Test premultiply:
    cout << "Test Premultiply:" << endl;
    cout << "COMPUTED: " << M1.preMultiply(in3, out3, true) << endl;
    cout
            << "EXPECTED: (| 5280 5280 5280 |,| -1110 -1110 -1110 |,| 2574 2574 2574 |,| -962 -962 -962 |,| 0 0 0 |,| 0 0 0 |)"
            << endl;
    //Now we need to do some big ones...
    srand(10); //Fix a seed for now.
    cout << "Creating a sparsity pattern for a 1000x1000 matrix." << endl;
    vector<vector<int> > spBIG(1000, vector<int> (100));
    int entries = 0;
    for (int i = 0; i < 1000; i++) {
        int len = 1 + (rand() % 100);
        //cout << "spBIG["<<i<<"]" << endl;
        for (int j = 0; j < len; j++) {
            spBIG[i][j] = rand() % 900 + 10;
        }
        sort(spBIG[i].begin(), spBIG[i].end());
        /*
         for (int j = 0; j < 100; j++) {
         cout << spBIG[i][j] << " ";

         }
         cout << endl;
         cout << "---" << endl;
         */
        entries += len;
    }
    cout << "Sparsity Pattern has " << entries
            << " nonzero entries, with duplicates." << endl;
    cout << "Creating a BIG 1000 by 1000 matrix." << endl;
    LargeMat3Matrix BIG(1000, 1000, spBIG, true);
    cout << "Matrix nonzero size (no dupes): " << BIG.getNonzeroSize() << endl;
    //cout << BIG;

    cout << "Creating a BIG 1000 long vector." << endl;
    LargeVec3Vector BIGin(1000, 2);
    LargeVec3Vector BIGout(1000);

    cout << "Lets do 2000 vector-matrix postmultiplies......" << endl;
    Timer timer;
    timer.Start();
     for (int i = 0; i < 2000; i++) {
       BIG.postMultiply(BIGin, BIGout);
     }
    cout << " >>> Done in " << timer.Stop() << " s" << endl;

    cout << "Lets do 2000 vector-matrix premultiplies......" << endl;
    timer.Start();
     for (int i = 0; i < 2000; i++) {
       BIG.preMultiply(BIGin, BIGout, true);
     }
    cout << " >>> Done in " << timer.Stop() << " s" << endl;
}

void testSimpleCG() {

    Timer timer;

    printHeading("SIMPLE TEST");

    vector<vector<int> > pattern(10);
    for (int i = 0; i < 10; i++) {
        pattern[i].push_back(i);
    }

    LargeMat3Matrix A1(10, 10, pattern, true);
    LargeVec3Vector b1(10, 5);
    LargeVec3Vector x1(b1.size(), 0);

    cout << "Ax = b with A of all identities, b of all 5. Expect x to be all 5." << endl;
    cout << "Running simpleCG." << endl;
    timer.Start();
    SimpleCG solver(b1.size());
    solver.solve(A1, b1, x1, 500, 0.0001);
    cout << "Done with simpleCG in " << timer.Stop() << " seconds." << endl;
    printResult("Result x1: " << x1);

    //-------------------------------------

    printHeading("CONTRIVED TEST");
    vector<vector<int> > pattern2(4);
    pattern2[0].push_back(0); pattern2[0].push_back(2); pattern2[0].push_back(3);
    pattern2[1].push_back(1); pattern2[1].push_back(2); pattern2[1].push_back(3);
    pattern2[2].push_back(0); pattern2[2].push_back(1); pattern2[2].push_back(2); pattern2[2].push_back(3);
    pattern2[3].push_back(0); pattern2[3].push_back(1); pattern2[3].push_back(2); pattern2[3].push_back(3);
    LargeMat3Matrix A2(4, 4, pattern2, false);

    mat3 m1;
    m1[0][0] = 0.1460; m1[0][1] = -0.0420; m1[0][2] = 0;
    m1[1][0] = -0.0420; m1[1][1] = 0.1460; m1[1][2] = 0;
    m1[2][0] = 0; m1[2][1] = 0; m1[2][2] = 0.02;

    A2(0,0) = m1;
    A2(1,1) = m1;

    m1[0][1] = m1[1][0] = 0.042;
    A2(2,2) = m1;

    mat3 m2;
    m2[0][0] = -0.0420; m2[0][1] = 0.0420; m2[0][2] = 0;
    m2[1][0] = 0.0420; m2[1][1] = -0.0420; m2[1][2] = 0;
    m2[2][0] = 0; m2[2][1] = 0; m2[2][2] = 0;

    A2(3,0) = m2;
    A2(3,1) = m2;
    A2(0,3) = m2;
    A2(1,3) = m2;

    mat3 m3;
    m3[0][0] = -0.0420; m3[0][1] = -0.0420; m3[0][2] = 0;
    m3[1][0] = -0.0420; m3[1][1] = -0.0420; m3[1][2] = 0;
    m3[2][0] = 0; m3[2][1] = 0; m3[2][2] = 0;

    A2(3,2) = m3;
    A2(2,3) = m3;

    A2(3,3)[0][0] = 0.188;
    A2(3,3)[1][1] = 0.188;
    A2(3,3)[2][2] = 0.02;

    A2(2, 0)[1][1] = -0.084;
    A2(2, 1)[0][0] = -0.084;

    A2(0, 2)[1][1] = -0.084;
    A2(1, 2)[0][0] = -0.084;

    cout << "Input A:" << endl;
    cout << A2 << endl;

    LargeVec3Vector b2(4, 0);
    vec3 temp(0, 0, -0.0039);
    b2[0] = temp; b2[1] = temp; b2[2] = temp; b2[3] = temp;

    cout << "Input b2: " << b2 << endl;
    LargeVec3Vector x2(b2.size(), 0);
    cout << "Running simpleCG." << endl;
    timer.Start();
    SimpleCG solver2(b2.size());
    int iter = solver2.solve(A2, b2, x2, 100, 0.00000001);
    cout << "Done with simpleCG in " << iter <<" steps, taking " << timer.Stop() << " seconds." << endl;
    printResult("Result x2: " << x2);

//    cout << endl << endl << endl;
//    A2.outAsMatlab(cout);


}

int main(int argc, char *argv[]) {

    printCategory("TEST LARGE VECTOR");
    testVector();

    printCategory("TEST LARGE MATRIX");
    testMatrix();

    testMatrixVectorMultiply();

    printCategory("SIMPLE CONJUGATE GRADIENT ITERATIVE SOLVER");
    testSimpleCG();

    cout << "TEST DONE" << endl;
}
