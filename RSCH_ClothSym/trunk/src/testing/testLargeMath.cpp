/*
 * testLargeMath.cpp
 *
 *  Created on: Oct 31, 2008
 *      Author: njoubert
 */

#include <iostream>
#include "../math/algebra3.h"
#include "../math/LargeVector.h"
#include "../math/LargeMatrix.h"

using namespace std;

void testVector() {
    cout << "TESTING DIFFERENT TYPES ASSIGNMENTS:" << endl;

        LargeVec3Vector v2(5);
        v2[0] = vec3(0,1,2);
        v2[1][0] = 3;
        v2[1][1] = 4;
        v2[1][2] = 5;
        cout << v2[0] << endl;
        cout << v2[1] << endl;

        cout << "TESTING OPERATORS:" << endl;

        LargeVec3Vector a1(5);
        LargeVec3Vector a2(5);
        a1[0] = a1[1] = a1[2] = a1[3] = vec3(2,3,4);
        a2[0] = a2[1] = a2[2] = a2[3] = vec3(3,4,5);
        cout << "a1 = " << a1 << endl;
        cout << "a2 = " << a2 << endl;
        a1 += a2;
        cout << "a1 += a2;" << endl;
        cout << "a1 = " << a1 << endl;

        LargeVec3Vector b1(4);
        LargeVec3Vector b2(4);
        for (int i = 0; i < 4; i++) {
            b1[i] = vec3(rand() % 100, rand() % 100, rand() % 100);
            b2[i] = vec3(i, i*10, i*100);
        }
        cout << "b1 = " << b1 << endl;
        cout << "b2 = " << b2 << endl;
        cout << "b1 += b2" << endl;
        b1 += b2;
        cout << "b1 = " << b1 << endl;

        cout << "TESTING COPY CONSTRUCTOR" << endl;
        LargeVec3Vector c1(b1);

        cout << "c1(b1)" << endl;
        cout << "c1 = " << c1 << endl;
        b1[1] = vec3(0,0,0);
        b1[2] = vec3(1,1,1);
        b1[3] = vec3(2,2,2);
        cout << "b1 = " << b1 << endl;
        cout << "c1 = " << c1 << endl;


        LargeVec3Vector * vp = new LargeVec3Vector(b1);

        cout << "v  = " << *vp << endl;
        delete vp;

        cout << "TESTING POINTERS" << endl;
        vec3* p = &b1[1];
        cout << "b1 = " << b1 << endl;
        (*p)[0] = 100000;
        (*p)[1] = 100000;
        cout << "b1 = " << b1 << endl;

}

void testMatrix() {
    LargeMat3Matrix v(5,5);

    cout << "v(5,5)" << endl;
    cout << "This should not be in the sparse row:" << endl;
    cout << "v(0,0) nonzero? " << v.isNonZero(0,0) << endl;

    cout << "Test sparsity pattern:" << endl;
    vector< vector<int> > pattern(10);
    for (int i = 0; i < 10; i++) {
        pattern[i].push_back(i);
    }

    cout << "LargeMat3Matrix s(10,10,pattern);" << endl;
    LargeMat3Matrix s(10,10,pattern, true);
    cout << s;

    cout << "Testing retrieval of elements:" << endl;
    cout << "s(1,1) = " << endl;
    cout << s(1,1) << endl;

}

void testMatrixVectorMultiply() {
    vector< vector<int> > pattern(10);
    for (int i = 0; i < 10; i++) {
        pattern[i].push_back(i);
    }

    LargeMat3Matrix s(10,10,pattern, true);
    LargeVec3Vector in(10, 5);
    LargeVec3Vector out(10);
    cout << "[10,10] Identity postMultiply(vector of all 5):" << endl;
    s.postMultiply(in, out);
    cout << " = " << out << endl;

    //-------------------------------------
    LargeVec3Vector inn(10,6);
    cout << "[10,10] Identity preMultiply(vector of all 6):" << endl;
    s.preMultiply(inn, out, true);
    cout << " = " << out << endl;

    //-------------------------------------


    cout << "Testing multiply with banded blocks of incrementing numbers." << endl;
    LargeMat3Matrix s2(10, 10, pattern, true);
    LargeVec3Vector in2(10,1);
    cout << "s2 = " << endl << s2 << endl;
    cout << "s2(0,0) = 0" << endl;
    s2(0,0) = mat3(0);
    cout << "s2(1,1) = 1" << endl;
    s2(1,1) = mat3(1);
    cout << "s2(2,2) = 2" << endl;
    s2(2,2) = mat3(2);
    cout << "s2(3,3) = 3" << endl;
    s2(3,3) = mat3(3);
    cout << "s2(4,4) = 4" << endl;
    s2(4,4) = mat3(4);
    cout << "s2(5,5) = 5" << endl;
    s2(5,5) = mat3(5);
    cout << "s2(6,6) = 6" << endl;
    s2(6,6) = mat3(6);
    cout << "s2(7,7) = 7" << endl;
    s2(7,7) = mat3(7);
    cout << "s2(8,8) = 8" << endl;
    s2(8,8) = mat3(8);
    cout << "s2(9,9) = 9" << endl;
    s2(9,9) = mat3(9);

    cout << "[10,10] banded postMultiply(vector of all 1):" << endl;
    s2.postMultiply(in2, out);
    cout << " = " << out << endl;

    //-------------------------------------

    LargeVec3Vector in2n(10,2);
    cout << "[10,10] banded preMultiply(vector of all 2):" << endl;
    s2.preMultiply(in2n, out, true);
    cout << " = " << out << endl;

    //-------------------------------------

    //Now to check some not-very-arbitrary sparse matrices...
    cout << "Creating a not-very-arbitrary sparse matrix pattern." << endl;
    vector< vector<int> > sp(6);
    sp[0].push_back(0); sp[0].push_back(2);
    sp[1].push_back(1); sp[1].push_back(3);
    sp[2].push_back(0); sp[2].push_back(2);
    sp[3].push_back(1); sp[3].push_back(3);
    LargeMat3Matrix M1(6,6,sp, true);
    cout << M1;
    cout << "Creating matrix." << endl;
    M1(0,0) = mat3(120); M1(0,2) = mat3(33);
    M1(1,1) = mat3(150); M1(1,3) = mat3(66);
    M1(2,0) = mat3(190); M1(2,2) = mat3(99);
    M1(3,1) = mat3(210); M1(3,3) = mat3(122);
    cout << M1 << endl;

    cout << "Creating a not-very-random vector." << endl;
    LargeVec3Vector in3(6);
    in3[0] = vec3(1,2,3);
    in3[1] = vec3(4,5,6);
    in3[2] = vec3(7,8,9);
    in3[3] = vec3(-1,-6,-9);
    in3[4] = vec3(-3,-7,39);
    in3[5] = vec3(-99,22,33);
    cout << "in = " << in3 << endl;

    LargeVec3Vector out3(6);

    //Test postmultiply:

    //Test premultiply:

    //Now we need to do some big ones...
    srand(10); //Fix a seed for now.
    cout << "Creating a sparsity pattern for a 1000x1000 matrix." << endl;
    vector< vector<int> > spBIG(1000, vector<int>(100));
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
    cout << "Sparsity Pattern has " << entries << " nonzero entries, with duplicates." << endl;
    cout << "Creating a BIG 1000 by 1000 matrix." << endl;
    LargeMat3Matrix BIG(1000,1000, spBIG, true);
    cout << "Matrix nonzero size (no dupes): " << BIG.getNonzeroSize() << endl;
    //cout << BIG;

    cout << "Creating a BIG 1000 long vector." << endl;
    LargeVec3Vector BIGin(1000, 2);
    LargeVec3Vector BIGout(1000);

    cout << "Lets do 1000 pre and post multiplies: " << endl;
    double starttime = clock()*1000/CLOCKS_PER_SEC;
    for (int i = 0; i < 500; i++) {
        //cout << "Lets to a premultiply:" << endl;
        BIG.preMultiply(BIGin, BIGout, true);
        //cout << "Lets do a postmultiply:" << endl;
        BIG.postMultiply(BIGin, BIGout);
    }
    double endtime = clock()*1000/CLOCKS_PER_SEC;
    cout << "Done in " << endtime - starttime << "ms" << endl;
}


int main(int argc, char *argv[]) {


    cout << "====================================" << endl;
    cout << "        TEST LARGE VECTOR" << endl;
    cout << "====================================" << endl;
    testVector();

    cout << "====================================" << endl;
    cout << "        TEST LARGE MATRIX" << endl;
    cout << "====================================" << endl;
    testMatrix();

    testMatrixVectorMultiply();

    cout << "TEST DONE" << endl;
}
