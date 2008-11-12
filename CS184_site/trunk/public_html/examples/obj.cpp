#include <vector>
#include <iostream>

using namespace std;

/* Demonstrates deep copy versus shallow copy */
int main() {
  vector<int> vec;
  vec.push_back(6);
  vector<int> vec2 = vec;
  
  cout << "End of vec = " << vec.back() << "\n";
  cout << "End of vec2 = " << vec2.back() << "\n";
  cout << "Modifying vec2...\n";
  vec2.push_back(12);
  cout << "End of vec = " << vec.back() << "\n";
  cout << "End of vec2 = " << vec2.back() << "\n";
  
  
  vector<int> *vecs = new vector<int>();
  vecs->push_back(3);
  vector<int> *vecs2 = vecs;

  cout << "End of *vecs = " << vecs->back() << "\n";
  cout << "End of *vecs2 = " << vecs2->back() << "\n";
  cout << "Modifying vecs2...\n";
  vecs2->push_back(1);
  cout << "End of *vecs = " << vecs->back() << "\n";
  cout << "End of *vecs2 = " << vecs2->back() << "\n";

  delete vecs;

}
