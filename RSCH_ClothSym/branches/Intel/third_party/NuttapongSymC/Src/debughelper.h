#pragma once

#include "globals.h"
#include "sparselib.h"
#include "mysparselib.h"
#include "mymatrix.h"
#include <fstream>
#include "linearstorage.h"

using namespace std;
//extern void WriteMFile(vector<VEC3>& vec, string name);
//extern void WriteMFile(vector<real>& vec, string name);
//extern void WriteMFile(Vector<real>& vec, string name);
//extern void WriteMFile(CCoorMatrix<real>& mat, string name);


template <class T> 
void WriteBin(vector<T>& vec, string name) {
   FILE* f = fopen(name.c_str(), "wb");
   size_t s = vec.size();
   fwrite(&s, sizeof(size_t), 1, f);
   fwrite(&vec[0], sizeof(T), s, f);
   fclose(f);
}

template <class T> 
void ReadBin(vector<T>& vec, string name) {
   FILE* f = fopen(name.c_str(), "rb");
   size_t s;
   fread(&s, sizeof(size_t), 1, f);
   vec.resize(s);
   fread(&vec[0], sizeof(T), s, f);
   fclose(f);
}

template <class T>
void PrintArray(vector<T>& vec, char* msg) {
   cout<<"---"<<msg<<"---"<<endl;
   for (int i = 0; i < vec.size(); i++) {
      cout<<i<<" : "<<vec[i]<<endl;
   }
   cout<<"----------"<<endl;
}

template <class T>
void PrintValue(T& a, char* msg) {
   cout<<"---"<<msg<<"---"<<endl;
   cout<<a<<endl;
   cout<<"----------"<<endl;
}

inline void OutForMCA(vector<VEC3>& vec, string name) {
   ofstream of(name.c_str());
   for (int i = 0; i < vec.size(); i++) {
      of<<vec[i][0]<<" "<<vec[i][1]<<" "<<vec[i][2]<<endl;
   }
   of.close();
}
/*inline void OutForMCA(vector<VEC2>& vec, string name) {
   ofstream of(name.c_str());
   for (int i = 0; i < vec.size(); i++) {
      of<<vec[i][0]<<" "<<vec[i][1]<<endl;
   }
   of.close();
}*/
inline void OutForMCA(vector<real>& vec, string name) {
   ofstream of(name.c_str());
   for (int i = 0; i < vec.size(); i++) {
      of<<vec[i]<<endl;
   }
   of.close();
}

inline void OutForMCA(int val, string name) {
   ofstream of(name.c_str());
   of<<val<<endl;
   of.close();
}
inline void OutForMCA(real val, string name) {
   ofstream of(name.c_str());
   of<<val<<endl;
   of.close();
}

/*inline void OutForMCA(vector<MATRIX2>& vec, string name) {
   ofstream of(name.c_str());
   for (int i = 0; i < vec.size(); i++) {
      MATRIX2& val = vec[i];
      of<<val[0][0]<<" "<<val[0][1]<<endl;
      of<<val[1][0]<<" "<<val[1][1]<<endl;
   }
   of.close();
}*/
inline void OutForMCA(vector<MATRIX3>& vec, string name) {
   ofstream of(name.c_str());
   for (int i = 0; i < vec.size(); i++) {
      MATRIX3& val = vec[i];
      of<<val[0][0]<<" "<<val[0][1]<<" "<<val[0][2]<<endl;
      of<<val[1][0]<<" "<<val[1][1]<<" "<<val[1][2]<<endl;
      of<<val[2][0]<<" "<<val[2][1]<<" "<<val[2][2]<<endl;
   }
   of.close();
}

// Binary file IO
/*
template <class T> 
void WriteVecBin(vector<T>& vec, FILE* f) {
   size_t s = vec.size();
   fwrite(&s, sizeof(size_t), 1, f);
   if (s > 0) {
      fwrite(&vec[0], sizeof(T), s, f);
   }
}

template <class T> 
void ReadVecBin(vector<T>& vec, FILE* f) {
   size_t s;
   fread(&s, sizeof(size_t), 1, f);
   vec.resize(s);
   if (s > 0) {
      fread(&vec[0], sizeof(T), s, f);
   }
}

template <class T> 
void WriteOneBin(T& t, FILE* f) {
   fwrite(&t, sizeof(T), 1, f);
}

template <class T> 
void ReadOneBin(T& t, FILE* f) {
   fread(&t, sizeof(T), 1, f);
}
*/
template <class T> 
void WriteVecBin(vector<T>& vec, CLinearStorage& f) {
   size_t s = vec.size();
   f.Write(&s, sizeof(size_t));
   if (s > 0) {
      f.Write(&vec[0], sizeof(T)*s);
   }
}

template <class T> 
void ReadVecBin(vector<T>& vec, CLinearStorage& f) {
   size_t s;
   f.Read(&s, sizeof(size_t));
   vec.resize(s);
   if (s > 0) {
      f.Read(&vec[0], sizeof(T)*s);
   }
}

template <class T> 
void WriteOneBin(T& t, CLinearStorage& f) {
   f.Write(&t, sizeof(T));
}

template <class T> 
void ReadOneBin(T& t, CLinearStorage& f) {
   f.Read(&t, sizeof(T));
}


// Text file IO
template <class T> 
void WriteVecTxt(vector<T>& vec, const string name, ofstream& f) {
   f<<name<<endl;
   size_t s = vec.size();
   f<<s<<endl;
   if (s > 0) {
      for (int i = 0; i < s; i++)
      f<<vec[i]<<endl;
   }
}

template <class T> 
void WriteOneTxt(T& t, const string name, ofstream& f) {
   f<<name<<endl;
   f<<t<<endl;
}

