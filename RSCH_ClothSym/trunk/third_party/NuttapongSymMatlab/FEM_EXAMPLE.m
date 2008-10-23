clear;
filename = 'mesh/bar.mesh';
[nodePos,tet2node,surfTri2node] = ReadInMesh(filename);
numSteps = 5000;

% Rubber

rho = 1.225;

E = 200;
v = 0.40;
lambda = v*E/((1+v)*(1-2*v))
mu = E/(2*(1+v))

% Random
alpha1 = 1e-7;
alpha2 = 1;

[K, M, Beta] = mex_FEM(uint32(0), nodePos, tet2node, lambda, mu, rho);
numValues = size(K,2);
% M = -M;
% C = -C;



C = (alpha1*K+alpha2*M);
% Display a mode

% Start simulation
figure(2), clf
g = -3;
dt = 1/100;


d = zeros(numValues, 1);
u = zeros(numValues, 1);

u_new = zeros(numValues, 1);

% Find nodes that are constrainted
constNodes = find(nodePos(1,:) < 0.01)
%constNodes = [];
%constNodes = find(nodePos(2,:) < -0.049);
plot3(nodePos(1,constNodes), nodePos(2,constNodes), nodePos(3,constNodes), 'r.');

const = [(constNodes-1)*3+1 (constNodes-1)*3+2 (constNodes-1)*3+3;];
%const = [];

% trisurf(surfTri2node',nodePos(1,:),nodePos(2,:),nodePos(3,:),1,'FaceAlpha',0.25);
% plot3(nodePos(1,constNodes), nodePos(2,constNodes), nodePos(3,constNodes), 'r.');
% axis equal;
% xlabel('x');
% ylabel('y');
% zlabel('z');
% keyboard;    

% Build mapping from solution (constraints removed) to full system.
globalToSol = ones(numValues, 1);
globalToSol(const) = 0;
globalToSol = max(1, cumsum(globalToSol));

applyNodes = find((nodePos(1,:) > -0.15));
%apply = [(applyNodes-1)*3+1 (applyNodes-1)*3+2 (applyNodes-1)*3+3];
apply = [(applyNodes-1)*3+2];


suspectNodes = find(nodePos(1,:) < 0.1);
suspect = [(suspectNodes-1)*3+1 (suspectNodes-1)*3+2 (suspectNodes-1)*3+3;];

linear = false;
curPos = nodePos;
cholIncDropTol_BICG = 1e-3;
tol_BICG = 1e-8;
maxIter_BICG = 200;
usol = zeros(numValues, 1);
for (step = 1:numSteps)
  step
  fext = zeros(numValues, 1);
  fext(2:3:numValues) = g;

    if (~linear)
        [K, M, force] = mex_FEM(uint32(4), nodePos, tet2node, lambda, mu, rho, d, Beta, curPos);     
        K = K + K';
        M = M + M';
        C = (alpha1*K+alpha2*M);
    end
     
   fext = M*fext;

    fext(const) = 0;
    
    %If you want nonzero velocity constraints, you need to put it here too.
    
    % Build A, b
     A = M/dt + C + 0.5*dt*K;
     if (~linear)
         b = M*u/dt - K*(0.5*dt*u) + fext + force;
     else
         b = M*u/dt - K*(d + 0.5*dt*u) + fext;
     end
     % Remove rows and column
     A(const, :) = [];
     A(:, const) = [];
     b(const) = [];
% IF use MATLAB \
     u_sol = A\b;
% IF use BCG
     % tic;
     % ordering = symamd(A);
     % tmpS = A(ordering, ordering);
     % tmpV = b(ordering);
     % Rtmp = cholinc(tmpS, cholIncDropTol_BICG);
     % chTime = toc;
     % fprintf('Cholesky + Reordering take %f\n', chTime);
     %tic;
     % xtmp = bicgstab(tmpS,tmpV,tol_BICG,maxIter_BICG,Rtmp',Rtmp);
     % usol(ordering) = xtmp;
     % bicgTime = toc;
     % fprintf('Bicg + sol reordering take %f, total time = %f\n', bicgTime, bicgTime + chTime);                 
     %u_new = usol(globalToSol);
     %u_new(const) = 0.0;
     u_new = u_sol(globalToSol);
     u_new(const) = 0.0;

     % 
     % Solve for d
     d = d + 0.5*dt*(u + u_new);
     u = u_new;
     curPos = nodePos + reshape(d, 3, numValues / 3);
     figure(1),clf, hold on

    
    trisurf(surfTri2node',curPos(1,:),curPos(2,:),curPos(3,:),1,'FaceAlpha',0.25);
    quiver3(curPos(1,:),curPos(2,:),curPos(3,:),force(1,:),force(2,:),force(3,:));
    
    axis equal;
    xlabel('x');
    ylabel('y');
    zlabel('z');
    view(2);
     axis([-.1 .6 -0.5 .30 -.35 .35]);

    figure(1)
end

