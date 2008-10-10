#ifndef __CONFIG_H__
#define __CONFIG_H__
#include "globals.h"
#include "linearstorage.h"
enum FORCE_METHOD{FORCE_GREEN, FORCE_IRVING, FORCE_CAUCHY, FORCE_MIXED_CI, FORCE_MIXED_GI, FORCE_NODE_ROT_LINEAR};
enum INTEGRATOR {EXPLICIT_EULER, IMPLICIT_EULER, EXPLICIT_MIDPOINT, IMPLICIT_NEWMARK, IMPLICIT_NEWMARK_COUPLE};

const  int numMaterials = 2;
class CConfig{
public:
   CConfig();
   void LoadConfig(string fname);
   void SaveStorage(CLinearStorage& f);
   void LoadStorage(CLinearStorage& f);

   FORCE_METHOD forceMethod; // Which force computation to use, not changable in config.txt
   INTEGRATOR integrator;  // Which integrator to use, not changable in config.txt
   string inputMesh; // Input mesh to use
   VEC3 gravity; // Gravity in m/s^2
   
   // Material properties
   real E[numMaterials];    //Young modulus in N/m^2, the more the stiffer, must be > 0, can go to very large eg. 10^10
   real v[numMaterials];    //Poisson ratio, how compressible the material is -1 to 0.5, mostly between 0 to 0.5
   real rho[numMaterials];  // Density, in kg/m^3
   real phi[numMaterials];  // Damp the change in volume
   real psi[numMaterials];  // Damp the shearing

   // Simulation parameter
   real dt; // Default time step
   real beta, gamma; // Parameters for Newmark integrator

   // Needle configs
   VEC3 needleInitPos; // Initial position of the needle outside tissue. At the first step, needle will be projected along direction needleInitForwardDir until hitting a tissue. A vertex will be created there by remeshing. The sim then start from there.
   VEC3 needleInitForwardDir; // Initial forward direction of the needle 
   VEC3 needleInitUpDir; // Initial up direction of the needle
   real needleBendAngularVel; // The speed in which the needle bend in, rad/s
   real needleVel; // Velocity of needle, will be replaced later by a function, m/s
   real needleAngularVel; // Spinning speed of needle, will be replaced later by a function, rad/s
   //IGNORE FOR NOW real fdis;  // Dissipasive friction force
   real vel0; // The velocity that would be considered 0 to turn dynamic friction to  friction, in m/s
   

   // Needle parameters that may not need to be changed now
   int pointLocationGiveUpCount; // The number of time to do tet walking before we give up, in point location
   real needleLFED; // Local frame estimated distance, as a fraction of velocity * dt
   

   // Parameters to keep simulation stable / Those that may not need to be changed now
   real svdThreshold; // Threshold of the singular value to declare that it is near 0 and hence we need to figure out the corresponding column of U to be orthonormal to other columns
   real baryThreshold; // Threshold of the diagonal entries of the bary-centric matrix for badly-shaped-in-material-space elements
   real irvingStressLimit; // The limit on the magnitude of stress, in order to avoid blowing up the simulation
   real mixDetThreshold; // In Mixed Force Mode, this is the threshold where the program starts to use the blend between Irving and Cauchy force
   real imixDetThreshold; // In Mixed Force Mode, this is the threshold where the program starts to use the blend between Irving and Cauchy force
   real velMulFactor; // Fake and cheap damping
   bool dampPhysical; // Use physicaOr not
   real lbQual;  // Lower bound on acceptable tet quality
   real giveupdt; // The minimum time step that we would give up
   real rcondLimit; //  Limit on the condition number of tet to not decay the young modulus
   real youngDecayFactor; // How fast young modulus decay when the condition number of an element exceed  rcondLimit

   // GUI parameters, Need not be changed now
   bool noRescaleNoTranslate; // No rescale and no translation to origin
   real sceneScale;  
   real mouseForceScale;
   real mouseForceScaleOldGUI;
   real mouseCutScaleOldGUI;
   real mouseMoveMul; // Multiplier to apply to needle movement constraint
   real objRndrSize;
   VEC3 objRndrDispWorld;
   VEC3 objRndrDispMat;

   // DER parameters
   real rhoDER;     // Density, in kg / m^3
   real R;          // Radius, in m
   real gammat;     // Dissipation coefficient
   real Es;         // Stretching stiffness 
   real alphaDER;   // Bending stiffness
   real betaDER;    // Twisting stiffness 
   real psiDER;     // Bending damping 
   real distanceFactorMerge; // When an edge is closer than distanceFactorMerge*totalNeedleLength, merge two nodes together
   real distanceFactorSplit; // When an edge is longer than distanceFactorSplit*totalNeedleLength, split the edge
//   real curvatureMerge;      // When curvature is lower than this, merge
//   real curvatureSplt;       // When curvature is higher than this, split

   // Coupling parameters
   real eta;         // Friction coefficient, friction force on needle = -eta*(v_n - v_t)                
   real fsmax;       // Maximum force before dynamic friction takes place
   real fsmaxPerLen; // Maximum force per unit length before dynamic friction takes place
   real fcut;        // Cut force
   int dampFrictionMode; // 0 no damp friction, 1 explicit damp friction, 2 implicit damp friction

   // Others
   int numSubsteps;  // Number of sub steps before rendering
   int explicitDER;  // Whether or not to do explicit integration for discrete elastic rod
   int enforceInextensibleMode; // 0 = none, 1 = fraction method, 2 = Pivot method

   real dampViscous; // Viscous damping coefficient
   vector<int> fixedNodes; // Nodes that are fixed during simulation
   real minFeatureSize; // This distance will be used as the lower bound in distance used to estimate local frame
};

#endif