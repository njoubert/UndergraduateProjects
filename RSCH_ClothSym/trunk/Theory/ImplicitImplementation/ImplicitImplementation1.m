%
% An implicit integrator and accompanying spring force 
% to simulate a particle system.
%

% We define the parameters of our simulation:
h = 0.01;        %timestep
ks = 20;          %spring constant
kd = 10;         %damping constant
dim = 3;                      %3d simulation

%initial velocities are all zero by default

% We start off by defining our system:
%1 square example:
%n = 5;
%x = [-1.0 -1.0 0  0.1 -1.0 0  -1.0 0.1 0  0.1 0.1 0  -0.45 -0.45 0.0];
%edges = [5 1; 1 2; 5 2; 2 4; 5 4; 4 3; 5 3; 3 1;];
%4 squares example:
n = 13;                        %three particle system
x = [-1.0 -1.0 0.0  0.0 -1.0 0.0  1.0 -1.0 0.0  -1.0 0.0 0.0  0.0 0.0 0.0  1.0 0.0 0.0  -1.0 1.0 0.0  0.0 1.0 0.0  1.0 1.0 0.0  -0.5 -0.5 0.0  0.5 -0.5 0.0  -0.5 0.5 0.0  0.5 0.5 0.0];
edges = [ 1 2; 10 1; 10 2 ; 10 4 ; 10 5 ; 11 2; 11 3 ; 11 5 ; 11 6 ; 12 4 ; 12 5 ; 12 7 ; 12 8 ; 13 5 ; 13 6 ; 13 8 ; 13 9 ; 2 3; 2 5 ; 3 6 ; 4 1; 4 5; 5 6; 5 8; 6 9; 7 4; 8 7; 9 8;];

v = zeros(1,n*dim);
M = 0.01*eye(n*dim, n*dim);        %Masses, 10g per particle
%for i=1:3:dim*n,
%    M(i:i+2,i:i+2) = rand()*.01*eye(3,3);      %randomize masses?
%end
Minv = inv(M);

t = 0;
axis manual;
axis([-.1 .6 -0.5 .30 -.35 .35]);
for j=1:100,
    %view(1);
    
%    plot3(x(1:3:9), x(2:3:9), x(3:3:9))
%    figure(1)

    %forces:
    f = zeros(1,n*dim);
    JP = zeros(n*dim,n*dim);
    JV = zeros(n*dim,n*dim);
    
    %look at each triangle...
    for i=1:length(edges(:,1)),
        %Get values for edges
       ia = (edges(i,1) - 1)*3 + 1;
       ib = (edges(i,2) - 1)*3 + 1;
       xa = x(ia:ia+2);
       xb = x(ib:ib+2);
       va = v(ia:ia+2);
       vb = v(ib:ib+2);
       
       rl = norm(xb-xa);
       %Calculate total force on each particle due to this triangle
       fa = fsa(xa,xb,rl,ks) + fda(xa,xb,va,vb,kd);
       fb = fsa(xb,xa,rl,ks) + fda(xb,xa,vb,va,kd);
       %Accumulate Forces
       f(ia:ia+2) = f(ia:ia+2) + fa;
       f(ib:ib+2) = f(ib:ib+2) + fb;
       
       %Calculate total jacobian wrt position and velocity for each
       %connection.
       JP_fa_xa = jdap(xa,xb,va,vb,rl,kd) + jsap(xa, xb, rl, ks);
       JP_fb_xa = -1*JP_fa_xa;
       JP_fb_xb = jdap(xb,xa,vb,va,rl,kd) + jsap(xb, xa, rl, ks);
       JP_fa_xb = -1*JP_fb_xb;
       
       JV_fa_xa = jdav(xa, xb, va, vb, rl, kd);
       JV_fb_xa = -1*JV_fa_xa;
       JV_fb_xb = jdav(xb, xa, vb, va, rl, kd);
       JV_fa_xb = -1*JV_fb_xb;
       
       %Accumulate Jacobians
       %remember, JP(force_i, position+i)
       JP(ia:ia+2,ia:ia+2) = JP(ia:ia+2,ia:ia+2) + JP_fa_xa;
       JP(ia:ia+2,ib:ib+2) = JP(ia:ia+2,ib:ib+2) + JP_fa_xb;
       JP(ib:ib+2,ia:ia+2) = JP(ib:ib+2,ia:ia+2) + JP_fb_xa;
       JP(ib:ib+2,ib:ib+2) = JP(ib:ib+2,ib:ib+2) + JP_fb_xb;
       JV(ia:ia+2,ia:ia+2) = JV(ia:ia+2,ia:ia+2) + JV_fa_xa;
       JV(ia:ia+2,ib:ib+2) = JV(ia:ia+2,ib:ib+2) + JV_fa_xb;
       JV(ib:ib+2,ia:ia+2) = JV(ib:ib+2,ia:ia+2) + JV_fb_xa;
       JV(ib:ib+2,ib:ib+2) = JV(ib:ib+2,ib:ib+2) + JV_fb_xb;
    end
    
    %EXPLICIT FORWARD EULER:
    %delX = h*v;
    %delV = f*inv(M);
    
    %IMPLICIT BACKWARDS EULER:
    A = eye(3*n) - h*Minv*JV - h^2*JP;
    b = h*(f + h*v*JP)*Minv;
    
    delV = b'\A;
    delX = h*(v + delV);
    
    x = x + delX;
    v = v + delV;
    
    
end

x'