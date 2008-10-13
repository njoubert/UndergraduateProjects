function [nodePos,tet2node,surfTri2node] = ReadInMesh(filename);

%readInMesh
%filename = 'unitCube_8';

disp(' ')
disp(['Reading in File: ' filename])

meshMat = textread(filename);

numNodes    = meshMat(1,1);
numTets     = meshMat(numNodes+2,1);
numSurfTri  = meshMat(numNodes+numTets+3,1);


nodePos = meshMat(2:numNodes+1,1:3);
tet2node = meshMat(numNodes+3:numNodes+2+numTets,2:5);
surfTri2node = meshMat(numNodes+4+numTets:end,2:4);


tet2node        = uint32(tet2node)';
surfTri2node    = uint32(surfTri2node)';
nodePos         = nodePos';
