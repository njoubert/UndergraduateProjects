#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

class CLinearStorage {
public:

   virtual void Reset() {};
   virtual void Write(const void* t, size_t num) {};

   virtual void Read(void* t, size_t num) {};
   virtual void Close() {};
};

class CLinearFileStorage : public CLinearStorage{
public:
   FILE* f;
   CLinearFileStorage(const string name, bool write) {
      if (write) {
         f = fopen(name.c_str(), "wb");
      } else {
         f = fopen(name.c_str(), "rb");
      }
      if (!f) {
         cout<<"File can't be opened"<<endl;
         exit(1);
      }
   }

   virtual void Reset() {
      fseek(f, 0, SEEK_SET);
   }

   virtual void Write(const void* t, size_t num) {
      fwrite(t, 1, num, f);
   }

   virtual void Read(void* t, size_t num) {
      fread(t, 1, num, f);
   }

   virtual void Close() {
      fclose(f);
   }
};

class CLinearMemStorage : public CLinearStorage{
public:
   vector<unsigned char>& v;
   size_t pos;
   virtual void Reset() {
      pos = 0;
   }
   virtual void Close() {
   }
   CLinearMemStorage(vector<unsigned char>& vi) : v(vi){
      pos = 0;
   }
   virtual void Write(const void* t, size_t num) {
      if (pos + num > v.size()) {
         v.resize(pos + num);
      }
      memcpy(&v[pos], t, num);
      pos += num;      
   }

   virtual void Read(void* t, size_t num) {
      memcpy(t, &v[pos], num);
      pos += num;
   }
};