function [force] = fsa(pa, pb, rl, ks)
% fsa(pa,pb,rl,ks): Calculates the elastic spring force between to
%   positions. vector pa, pb, scalar rl, ks.

force = -1*ks*((pa-pb)/norm(pa-pb))*(norm(pa-pb) - rl);

