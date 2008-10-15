%
% An implicit integrator and accompanying spring force 
% to simulate a particle system.
%

% We define the parameters of our simulation:
h = 0.01;        %timestep
ks = 50;          %spring constant
kd = .1;         %damping constant
rl = 0.3;
% We start off by defining our system:
n = 3;                        %three particle system
dim = 3;                      %3d simulation
x = [0 0 0  1 0 1  1 2 0];    %positions
v = [0 0 0  0 0 0  0 0 0];    %velocities
M = eye(n*dim, n*dim);        %Masses
M(1:3,1:3) = 1*eye(3,3);      %particle one is 1kg
M(4:6,4:6) = 2*eye(3,3);      %particle two is 2kg
M(7:9,7:9) = 3*eye(3,3);      %particle three is 3kg
Minv = inv(M);

% ====================================
% Forces between these three particles.
% our setup is as follows:
% 
% ====================================
t = 0;
axis manual;
axis([-.1 .6 -0.5 .30 -.35 .35]);
for j=1:10,
    %view(1);
    
    plot3(x(1:3:9), x(2:3:9), x(3:3:9))
    figure(1)
    x1 = x(1:3);
    x2 = x(4:6);
    x3 = x(7:9);
    v1 = v(1:3);
    v2 = v(4:6);
    v3 = v(7:9);
    f12 = fsa(x1,x2,rl,ks) + fda(x1,x2,v1,v2,kd);
    f23 = fsa(x2,x3,rl,ks) + fda(x2,x3,v2,v3,kd);
    f13 = fsa(x1,x3,rl,ks) + fda(x1,x3,v1,v3,kd);
    j12p = jdap(x1,x2,v1,v2,rl,kd) + jsap(x1, x2, rl, ks);
    j23p = jdap(x2,x3,v2,v3,rl,kd) + jsap(x2, x3, rl, ks);
    j13p = jdap(x1,x3,v1,v3,rl,kd) + jsap(x1, x3, rl, ks);
    j12v = jdav(x1, x2, v1, v2, rl, kd);
    j23v = jdav(x2, x3, v2, v3, rl, kd);
    j13v = jdav(x1, x3, v1, v3, rl, kd);

    f1 = f12 + f13;
    f2 = -1*f12 + f23;
    f3 = -1*f23 + -1*f13;
    %FORCE VECTOR
    f = [f1 f2 f3];

    %JACOBIAN MATRIX
    JP = [j12p -j12p zeros(3); zeros(3) j23p -j23p; j13p zeros(3) -j13p];
    JV = [j12v -j12v zeros(3); zeros(3) j23v -j23v; j13v zeros(3) -j13v];
    
    %EXPLICIT FORWARD EULER:
    %delX = h*v;
    %delV = f*inv(M);
    
    %IMPLICIT BACKWARDS EULER:
    A = eye(3*n) - h*Minv*JV - h^2*JP;
    b = h*(f + h*v*JP)*Minv;
    
    delV = b*inv(A);
    delX = h*(v + delV);
    
    x = x + delX;
    v = v + delV;
    
end