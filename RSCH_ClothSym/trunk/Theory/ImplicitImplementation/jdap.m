function [jacobian] = jdap(pa, pb, va, vb, rl, kd)
% fsa(pa,pb,rl,ks): Calculates the elastic damping force between to
%   positions. row vector pa, pb, va, vb, scalar rl, ks.
outerPP = (pa-pb)' * (pa-pb);  %outer product between positon and position
outerPV = (pa-pb)' * (va-vb);  %outer product between position and velocity
dotPP = dot((pa-pb),(pa-pb));
dotPV = dot((pa-pb),(va-vb));

a = outerPV / dotPP;
b = 2*((-1*dotPV)/(dotPP)^2)*outerPP;
c = (dotPV)/(dotPP)*eye(3);

jacobian = -1*kd*(a + b + c);