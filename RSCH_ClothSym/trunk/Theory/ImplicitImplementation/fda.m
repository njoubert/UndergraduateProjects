function [force] = fda(pa, pb, va, vb, kd)
%   fda:    Calculates the damping force

force = -kd*(dot((pa-pb),(va-vb))/(norm(pa-pb)^2))*(pa-pb);