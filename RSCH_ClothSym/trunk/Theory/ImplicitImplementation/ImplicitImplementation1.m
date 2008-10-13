%
% An implicit integrator and accompanying spring force 
% to simulate a particle system.
%

% We define the parameters of our simulation:
h = 0.02;        %timestep
ks = 0;          %spring constant
kd = 0;         %damping constant

% We start off by defining our system:
n = 2;                        %two particle system
dim = 3;                      %3d simulation
x = [0 0 0 1 1 1]';            %positions
v = [0 0 0 0 0 0]';            %velocities
M = eye(n*dim, n*dim);        %Masses
M(1:3,1:3) = 1*eye(3,3);      %particle one is 1kg
M(4:6,4:6) = 2*eye(3,3);      %particle two is 2kg

% ====================================
% Forces between these two particles:
% ====================================


