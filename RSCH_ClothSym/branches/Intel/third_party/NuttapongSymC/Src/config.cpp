#include <fstream>
#include <sstream>
#include "config.h"
#include "debughelper.h"
CConfig::CConfig() {
   forceMethod = FORCE_IRVING;
   //integrator = EXPLICIT_EULER; 
   //forceMethod = FORCE_CAUCHY;
   integrator = IMPLICIT_NEWMARK; 
   inputMesh = "thickplate.mesh";
   objRndrSize = 1.5;
   objRndrDispWorld = VEC3(1.5, 0.0, 0.0);
   objRndrDispMat = VEC3(-1.5, 0.0, 0.0);

   needleInitForwardDir = VEC3(1,0,0);
   needleInitUpDir = VEC3(0,1,0);
   sceneScale = 0.05;
   gravity = VEC3(0,-10,0);
   E[0] = 30e3;
   E[1] = 60e3;    //Young modulus, the more the stiffer, must be > 0, can go to very large eg. 10^10

   v[0] = 0.45;
   v[1] = 0.45;
   
   rho[0] = 1000;
   rho[1] = 1000;  // Density
   phi[0] = 0.01;
   phi[1] = 0.01;
   psi[0] = 0.01;
   psi[1] = 0.01;
   svdThreshold = 1e-3;
   baryThreshold = 50000000;
   irvingStressLimit = 1e20;
   mixDetThreshold = 0.5;
   imixDetThreshold = 1.0f / mixDetThreshold;

   velMulFactor = 1; // Fake and cheap damping
   dampPhysical = true; // Or not
   dt = 0.00002;
   mouseForceScale = 0.001f;
   mouseCutScaleOldGUI = 0.1f;
   mouseForceScaleOldGUI = 0.01f;
   lbQual = 1;
   giveupdt = dt / 16;

   // Needle properties
   needleInitPos = VEC3(-10,-10,-10);
   needleBendAngularVel = 500*MYPI/6; // The direction in which the needle will move, with respect to its coordinate system, a unit vector
   needleVel = 10; // Velocity of needle 
   needleAngularVel = 0;//20*PI; // Spinning speed of needle
   fsmax = 2; // Maximum force before dynamic friction takes place
   fcut = 20;  // Cut force
//   fdis = 5;  // Dissipasive friction force
   needleLFED = 2;
   pointLocationGiveUpCount = 10;
   vel0 = 1e-2;
   rcondLimit = 10;
   youngDecayFactor = 1;
   beta = 0.25;
   gamma = 0.5;
   noRescaleNoTranslate = false;

   // DER coefficient

   rhoDER = 1300;
   R = 0.1;
   gammat = 0;
   Es = 2e9;
   alphaDER = 1.5e7;
   psiDER = 1e-1;
   betaDER = 1000;
   eta = 0.1;
   dampFrictionMode = 2; // 0 = no damp, 1 = explicit, 2 = implicit
   numSubsteps = 1;
   explicitDER = 0;
   enforceInextensibleMode = 0;
   dampViscous = 0;
   mouseMoveMul = 0.003;

   distanceFactorMerge = 0.1;
   distanceFactorSplit = 4;
   fsmaxPerLen = 2;     
   minFeatureSize = 1e-6;
}

void CConfig::LoadConfig(string fname) {
   ifstream input(fname.c_str(), ios::in);
   string param;
   while (input >> param) {
      if (param[0] == '#') input.ignore(100000, '\n'); else
      if (param == string("inputMesh")) input>>inputMesh; else
      if (param == string("gravity")) input>>gravity[0]>>gravity[1]>>gravity[2]; else
      if (param == string("E")) {for (int i = 0; i < numMaterials; i++) input>>E[i];} else
      if (param == string("v")) {for (int i = 0; i < numMaterials; i++) input>>v[i];} else
      if (param == string("rho")) {for (int i = 0; i < numMaterials; i++) input>>rho[i];} else
      if (param == string("phi")) {for (int i = 0; i < numMaterials; i++) input>>phi[i];} else
      if (param == string("psi")) {for (int i = 0; i < numMaterials; i++) input>>psi[i];} else
      if (param == string("svdThreshold")) input>>svdThreshold; else
      if (param == string("baryThreshold")) input>>baryThreshold; else
      if (param == string("irvingStressLimit")) input>>irvingStressLimit; else
      if (param == string("mixDetThreshold")) {input>>mixDetThreshold; imixDetThreshold = 1.0 / mixDetThreshold; }else      
      if (param == string("velMulFactor")) input>>velMulFactor; else
      if (param == string("dampPhysical")) input>>dampPhysical; else
      if (param == string("sceneScale")) input>>sceneScale; else
      if (param == string("dt")) input>>dt; else
      if (param == string("mouseForceScale")) input>>mouseForceScale; else
      if (param == string("mouseForceScaleOldGUI")) input>>mouseForceScaleOldGUI; else
      if (param == string("mouseCutScaleOldGUI")) input>>mouseCutScaleOldGUI; else
      if (param == string("mouseMoveMul")) input>>mouseMoveMul; else
      if (param == string("lbQual")) input>>lbQual; else
      if (param == string("giveupdt")) input>>giveupdt; else
      if (param == string("numSubsteps")) input>>numSubsteps; else
      if (param == string("rcondLimit")) input>>rcondLimit; else
      if (param == string("youngDecayFactor")) input>>youngDecayFactor; else

      // Needle configs
      if (param == string("needleBendAngularVel")) input>>needleBendAngularVel; else
      if (param == string("needleVel")) input>>needleVel; else
      if (param == string("needleAngularVel")) input>>needleAngularVel; else
      //if (param == string("fdis")) input>>fdis; else
      if (param == string("needleLFED")) input>>needleLFED; else
      if (param == string("pointLocationGiveUpCount")) input>>pointLocationGiveUpCount; else
      if (param == string("vel0")) input>>vel0; else
      if (param == string("gamma")) input>>gamma; else
      if (param == string("beta")) input>>beta; 
      if (param == string("needleInitPos")) input>>needleInitPos[0]>>needleInitPos[1]>>needleInitPos[2]; else
      if (param == string("needleInitForwardDir")) input>>needleInitForwardDir[0]>>needleInitForwardDir[1]>>needleInitForwardDir[2]; else
      if (param == string("needleInitUpDir")) input>>needleInitUpDir[0]>>needleInitUpDir[1]>>needleInitUpDir[2]; else
      if (param == string("noRescaleNoTranslate")) input>>noRescaleNoTranslate; else
      if (param == string("minFeatureSize")) input>>minFeatureSize; else
      

      // DER config
      if (param == string("rhoDER")) input>>rhoDER; else
      if (param == string("R")) input>>R; else
      if (param == string("gammat")) input>>gammat;  else
      if (param == string("Es")) input>>Es; else
      if (param == string("alphaDER")) input>>alphaDER; else
      if (param == string("betaDER")) input>>betaDER; else
      if (param == string("psiDER")) input>>psiDER; else 
      if (param == string("distanceFactorMerge")) input>>distanceFactorMerge; else 
      if (param == string("distanceFactorSplit")) input>>distanceFactorSplit; else 
     
      // Render config
      if (param == string("objRndrSize")) input>>objRndrSize; else
      if (param == string("objRndrDispWorld")) input>>objRndrDispWorld[0]>>objRndrDispWorld[1]>>objRndrDispWorld[2]; else
      if (param == string("objRndrDispMat")) input>>objRndrDispMat[0]>>objRndrDispMat[1]>>objRndrDispMat[2]; else

      // Coupling config
      if (param == string("fsmax")) input>>fsmax; else
      if (param == string("fsmaxPerLen")) input>>fsmaxPerLen; else
      if (param == string("fcut")) input>>fcut; else
      if (param == string("eta")) input>>eta; else
      if (param == string("dampFrictionMode")) input>>dampFrictionMode; else
        
      // Others
      if (param == string("explicitDER")) input>>explicitDER; else
      if (param == string("enforceInextensibleMode")) input>>enforceInextensibleMode; else
      if (param == string("dampViscous")) input>>dampViscous; 

      // fixed nodes
      if (param == string("fixedNodes")) {
         string str;
         getline(input, str, '\n');
         int len = str.length()-1;
         while (str[len] == ' ') {
            len--;
         }
         str.erase(len+1);
         istringstream istr(str);
         fixedNodes.clear();
         int num;
         istr>>num;
         while (!istr.eof()) {
            fixedNodes.push_back(num);
            istr>>num;
         }
         fixedNodes.push_back(num);
/*
         cout<<"fixNodes";
         for (int i = 0; i < fixedNodes.size(); i++) cout<<fixedNodes[i]<<" ";
         cout<<endl;
  */          


      }
         
   }
   input.close();

}

void CConfig::SaveStorage(CLinearStorage& f) {
   
   WriteOneBin( forceMethod, f);
   WriteOneBin( integrator, f); 
   size_t s = inputMesh.size();
   WriteOneBin(s, f);
   f.Write(&inputMesh[0], sizeof(char)*s);
   WriteOneBin( gravity, f); 
   
   // Material properties
   for (int i = 0; i < numMaterials; i++) {
      WriteOneBin(E[i], f);  
      WriteOneBin(v[i], f);   
      WriteOneBin(rho[i], f); 
      WriteOneBin(phi[i], f); 
      WriteOneBin(psi[i], f); 
   }

   // Simulation parameter
   WriteOneBin( dt, f); 
   WriteOneBin( beta, f); 
   WriteOneBin(gamma, f);

   // Needle configs
   WriteOneBin( needleInitForwardDir, f); 
   WriteOneBin( needleInitUpDir, f);
   WriteOneBin( needleBendAngularVel, f); 
   WriteOneBin( needleVel, f); 
   WriteOneBin( needleAngularVel, f); 
   //IGNORE FOR NOW real fdis;  // Dissipasive friction force
   WriteOneBin( vel0, f); 
   

   // Needle parameters that may not need to be changed now
   WriteOneBin( pointLocationGiveUpCount, f); 
   WriteOneBin( needleLFED, f); 
   

   // Parameters to keep simulation stable / Those that may not need to be changed now
   WriteOneBin( svdThreshold, f); 
   WriteOneBin( baryThreshold, f); 
   WriteOneBin( irvingStressLimit, f);
   WriteOneBin( mixDetThreshold, f); 
   WriteOneBin( imixDetThreshold, f); 
   WriteOneBin( velMulFactor, f); 
   WriteOneBin( dampPhysical, f); 
   WriteOneBin( lbQual, f);  
   WriteOneBin( giveupdt, f); 
   WriteOneBin( rcondLimit, f); 
   WriteOneBin( youngDecayFactor, f); 

   // GUI parameters, Need not be changed now
   WriteOneBin( noRescaleNoTranslate, f);
   WriteOneBin( sceneScale, f);  
   WriteOneBin( mouseForceScale, f);
   WriteOneBin( mouseForceScaleOldGUI, f);
   WriteOneBin( mouseCutScaleOldGUI, f);
   WriteOneBin( mouseMoveMul, f); 
   WriteOneBin( objRndrSize, f);
   WriteOneBin( objRndrDispWorld, f);
   WriteOneBin( objRndrDispMat, f);

   // DER parameters
   WriteOneBin( rhoDER, f);    
   WriteOneBin( R, f);         
   WriteOneBin( gammat, f);    
   WriteOneBin( Es, f);        
   WriteOneBin( alphaDER, f);  
   WriteOneBin( betaDER, f);   
   WriteOneBin( psiDER, f);    
   WriteOneBin( distanceFactorMerge, f);
   WriteOneBin( distanceFactorSplit, f);
//   real curvatureMerge;      // When curvature is lower than this, merge
//   real curvatureSplt;       // When curvature is higher than this, split

   // Coupling parameters
   WriteOneBin( eta, f);         
   WriteOneBin( fsmax, f);       
   WriteOneBin( fsmaxPerLen, f); 
   WriteOneBin( fcut, f);        
   WriteOneBin( dampFrictionMode, f); 

   // Others
   WriteOneBin( numSubsteps, f);  
   WriteOneBin( explicitDER, f);  
   WriteOneBin( enforceInextensibleMode, f);

   WriteOneBin( dampViscous, f);
   WriteOneBin( needleInitPos, f);
   WriteVecBin( fixedNodes, f);

}

void CConfig::LoadStorage(CLinearStorage& f) {
   ReadOneBin( forceMethod, f);
   ReadOneBin( integrator, f); 
   size_t s;
   ReadOneBin(s, f);
   char* nn = new char[s+1];
   f.Read(nn, s);
   nn[s] = '\0';
   inputMesh = nn;
   SAFE_ARRAY_DELETE(nn);
   //inputMesh.resize(s)
   //f.Write(&inputMesh[0], sizeof(char)*s);
   ReadOneBin( gravity, f); 
   
   // Material properties
   for (int i = 0; i < numMaterials; i++) {
      ReadOneBin(E[i], f);  
      ReadOneBin(v[i], f);   
      ReadOneBin(rho[i], f); 
      ReadOneBin(phi[i], f); 
      ReadOneBin(psi[i], f); 
   }

   // Simulation parameter
   ReadOneBin( dt, f); 
   ReadOneBin( beta, f); 
   ReadOneBin(gamma, f);

   // Needle configs
   ReadOneBin( needleInitForwardDir, f); 
   ReadOneBin( needleInitUpDir, f);
   ReadOneBin( needleBendAngularVel, f); 
   ReadOneBin( needleVel, f); 
   ReadOneBin( needleAngularVel, f); 
   //IGNORE FOR NOW real fdis;  // Dissipasive friction force
   ReadOneBin( vel0, f); 
   

   // Needle parameters that may not need to be changed now
   ReadOneBin( pointLocationGiveUpCount, f); 
   ReadOneBin( needleLFED, f); 
   

   // Parameters to keep simulation stable / Those that may not need to be changed now
   ReadOneBin( svdThreshold, f); 
   ReadOneBin( baryThreshold, f); 
   ReadOneBin( irvingStressLimit, f);
   ReadOneBin( mixDetThreshold, f); 
   ReadOneBin( imixDetThreshold, f); 
   ReadOneBin( velMulFactor, f); 
   ReadOneBin( dampPhysical, f); 
   ReadOneBin( lbQual, f);  
   ReadOneBin( giveupdt, f); 
   ReadOneBin( rcondLimit, f); 
   ReadOneBin( youngDecayFactor, f); 

   // GUI parameters, Need not be changed now
   ReadOneBin( noRescaleNoTranslate, f);
   ReadOneBin( sceneScale, f);  
   ReadOneBin( mouseForceScale, f);
   ReadOneBin( mouseForceScaleOldGUI, f);
   ReadOneBin( mouseCutScaleOldGUI, f);
   ReadOneBin( mouseMoveMul, f); 
   ReadOneBin( objRndrSize, f);
   ReadOneBin( objRndrDispWorld, f);
   ReadOneBin( objRndrDispMat, f);

   // DER parameters
   ReadOneBin( rhoDER, f);    
   ReadOneBin( R, f);         
   ReadOneBin( gammat, f);    
   ReadOneBin( Es, f);        
   ReadOneBin( alphaDER, f);  
   ReadOneBin( betaDER, f);   
   ReadOneBin( psiDER, f);    
   ReadOneBin( distanceFactorMerge, f);
   ReadOneBin( distanceFactorSplit, f);
//   real curvatureMerge;      // When curvature is lower than this, merge
//   real curvatureSplt;       // When curvature is higher than this, split

   // Coupling parameters
   ReadOneBin( eta, f);         
   ReadOneBin( fsmax, f);       
   ReadOneBin( fsmaxPerLen, f); 
   ReadOneBin( fcut, f);        
   ReadOneBin( dampFrictionMode, f); 

   // Others
   ReadOneBin( numSubsteps, f);  
   ReadOneBin( explicitDER, f);  
   ReadOneBin( enforceInextensibleMode, f);

   ReadOneBin( dampViscous, f);
   ReadOneBin( needleInitPos, f);
   ReadVecBin( fixedNodes, f);

}
