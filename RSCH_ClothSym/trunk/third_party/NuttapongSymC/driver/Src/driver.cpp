#include "needlesimdll.h"
int main(int argc, char** argv) {
   cout<<"ye"<<endl;
   NeedleSimInterface inf;
   inf.LoadModel("config.txt");
   vector<unsigned char> cv;
   ControlInputWrapper control(cv);
   control.SetControl(0.0, 0, 0);
   VEC3 pos;
   control.SetControl(0.05, 0, 0);
   inf.Simulate(cv, 0.01);
   control.SetControl(0.0, 0, 0);
   inf.Simulate(cv, 0.01);
   for (int i = 0; i < 10; i++) {
      control.SetControl(0.05, 0, 0);
      inf.Simulate(cv, 0.01);
      inf.GetCurrentNeedleTipPosition(pos);
      cout<<pos<<endl;
   }
   
   return 0;
}
