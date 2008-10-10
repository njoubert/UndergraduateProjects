function [index, bary] = findTet(point, Beta)
numTets = size(Beta, 2);
p = [point; 1];
for (i = 1:numTets)
    mat = reshape(Beta(:, i),4,4)';
    bb = mat*p;
    if (sum(bb) == 1) 
        if (min(bb) >= -1e-8)
%            if (max(bcoord) <= 1+1e-8)
                % Found it
                bary = bb;
                index = i;            
 %           end
        end
    end
    
    
end