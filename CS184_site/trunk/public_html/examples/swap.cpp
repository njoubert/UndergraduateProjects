#include <iostream>\
using namespace std;

/* Demonstrates swapping using pointers. */

void swap(int &a, int &b) {
  int t = a;
  a = b;
  b = t;

}

int main() {

  int one = 1;
  int two = 2;

  cout << "One is " << one << " and two is " << two << "\n";
  swap(one, two);
  cout << "One is " << one << " and two is " << two << "\n";

  return 0;
  
}
