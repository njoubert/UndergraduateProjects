function [jacobian] = jsap(pa, pb, rl, ks)
% fsa(pa,pb,rl,ks): Calculates the elastic spring force between to
%   positions. row vector pa, pb, scalar rl, ks.
outerP = (pa-pb)' * (pa-pb);
lenSquared = dot((pa-pb),(pa-pb));
len = norm(pa-pb);
a = (1 - rl/len) * (eye(3) - outerP/lenSquared);
b = outerP / lenSquared;

jacobian = -1*ks*(a + b);