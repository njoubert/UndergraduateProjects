/*
 * testLargeMath.cpp
 *
 *  Created on: Oct 31, 2008
 *      Author: njoubert
 */

#include <iostream>
#include "../../include/algebra3.h"
#include "../math/LargeVector.h"
#include "../math/LargeMatrix.h"

using namespace std;

int main(int argc, char *argv[]) {
    cout << "TESTING DIFFERENT TYPES ASSIGNMENTS:" << endl;

    cout << "Ints:" << endl;
    LargeVector<int> v(5);
    v[0] = 1;
    cout << v[0] << endl;
    v[4] = 1;
    cout << v[4] << endl;

    cout << "Vec3:" << endl;
    LargeVector<vec3> v2(5);
    v2[0] = vec3(0,1,2);
    v2[1][0] = 3;
    v2[1][1] = 4;
    v2[1][2] = 5;
    cout << v2[0] << endl;
    cout << v2[1] << endl;
    //LargeMatrix<int> m(5,5);

    cout << "Mat4:" << endl;
    LargeVector<mat4> v3(250);
    v3[55] = identity3D();
    cout << v3[54] << endl;
    cout << v3[55] << endl;
    cout << v3[56] << endl;

    cout << "TESTING OPERATORS:" << endl;

    LargeVector<int> a1(5);
    LargeVector<int> a2(5);
    a1[0] = a1[1] = a1[2] = a1[3] = 5;
    a2[0] = a2[1] = a2[2] = a2[3] = 6;
    cout << "a1 = " << a1 << endl;
    cout << "a2 = " << a2 << endl;
    a1 += a2;
    cout << "a1 += a2;" << endl;
    cout << "a1 = " << a1 << endl;

    LargeVector<vec3> b1(4);
    LargeVector<vec3> b2(4);
    for (int i = 0; i < 4; i++) {
        b1[i] = vec3(rand() % 100, rand() % 100, rand() % 100);
        b2[i] = vec3(i, i*10, i*100);
    }
    cout << "b1 = " << b1 << endl;
    cout << "b2 = " << b2 << endl;
    cout << "b1 += b2" << endl;
    b1 += b2;
    cout << "b1 = " << b1 << endl;


    cout << "TEST DONE" << endl;
}
