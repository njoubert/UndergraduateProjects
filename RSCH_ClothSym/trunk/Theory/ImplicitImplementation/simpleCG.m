function [x] = simpleCG(A, b, x, imax, e)

i = 0;
r = b - x*A;
d = r;
deln = r*r';
del0 = deln;

while (i < imax && deln > e*e*del0)
    
    q = d*A;
    alpha = (deln)/ (d*q');

    x = x + alpha*d;
    r = b - x*A;
    
    del0 = deln;
    deln = r*r';
    beta = deln / del0;
    d = r + beta*d;
    i = i + 1;
    
    
end

x;