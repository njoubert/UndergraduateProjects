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
    /*
    cout << "v(3,3)" << endl;
    v(3,3);
    cout << "v(6,6)" << endl;
    v(6,6);
    */
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

    cout << "TEST DONE" << endl;
}
