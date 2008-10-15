function [force] = fda(pa, pb, va, vb, kd)
%   fda:    Calculates the damping force between two particles
%           connected with a spring. vectors pa, pb, va, vb, scalar kd   

force = -kd*(dot((pa-pb),(va-vb))/(norm(pa-pb)^2))*(pa-pb);