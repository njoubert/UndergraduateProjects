//#define __MEM_CHECK__
#ifdef __MEM_CHECK__
#define NEW_ARRAY(tmp, a, s) tmp = new a[s]; allocate<<(MYINT)tmp<<" "<<__LINE__<<endl
#define NEW(tmp, a) tmp = new a(); allocate<<(MYINT)tmp<<" "<<__LINE__<<endl
#define DELETE(tmp) deallocate<<(MYINT)tmp<<" "<<__LINE__<<endl; delete tmp
#define DELETE_ARRAY(tmp) deallocate<<(MYINT)tmp<<" "<<__LINE__<<endl; delete [] tmp

#else
#define NEW_ARRAY(tmp, a, s) tmp = new a[s]
#define NEW(tmp, a) tmp = new a()
#define DELETE(tmp)  delete tmp
#define DELETE_ARRAY(tmp) delete [] tmp

#endif

double mmax;
double mmin;

class CBBox{
public:
	CBBox() {
		mins[0] = mins[1] = mins[2] = 1e50;
		maxs[0] = maxs[1] = maxs[2] = -1e50;
	}
	CBBox& operator = (const CBBox& b) {
		mins[0] = b.mins[0];
		mins[1] = b.mins[1];
		mins[2] = b.mins[2];

		maxs[0] = b.maxs[0];
		maxs[1] = b.maxs[1];
		maxs[2] = b.maxs[2];
		return *this;
	}
	CBBox(const double& mx, const double& my, const double& mz, const double& Mx, const double& My, const double& Mz) {
		mins[0] = mx; mins[1] = my; mins[2] = mz;
		maxs[0] = Mx; maxs[1] = My; maxs[2] = Mz;
	}
	inline void unionWith(CBBox& a) {
		mins[0] = min(mins[0],a.mins[0]);
		mins[1] = min(mins[1],a.mins[1]);
		mins[2] = min(mins[2],a.mins[2]);

		maxs[0] = max(maxs[0],a.maxs[0]);
		maxs[1] = max(maxs[1],a.maxs[1]);
		maxs[2] = max(maxs[2],a.maxs[2]);
	}
	inline bool inside(CBBox& a) {
		// Check if this bounding box is inside bbox a
		return (mins[0] >= a.mins[0]) && (maxs[0] <= a.maxs[0]) && 
			   (mins[1] >= a.mins[1]) && (maxs[1] <= a.maxs[1]) && 
			   (mins[2] >= a.mins[2]) && (maxs[2] <= a.maxs[2]);								
	}
	inline bool insideStrictly(CBBox& a) {
		// Check if this bounding box is inside bbox a
		return (mins[0] > a.mins[0]) && (maxs[0] < a.maxs[0]) && 
			   (mins[1] > a.mins[1]) && (maxs[1] < a.maxs[1]) && 
			   (mins[2] > a.mins[2]) && (maxs[2] < a.maxs[2]);								
	}
	inline bool contain(double* p) {
		return ((mins[0] <= p[0]) && (p[0] <= maxs[0]) &&
				(mins[1] <= p[1]) && (p[1] <= maxs[1]) &&
				(mins[2] <= p[2]) && (p[2] <= maxs[2]));
	}
	inline bool containStrictly(double* p) {
		return ((mins[0] < p[0]) && (p[0] < maxs[0]) &&
				(mins[1] < p[1]) && (p[1] < maxs[1]) &&
				(mins[2] < p[2]) && (p[2] < maxs[2]));
	}
	inline bool intersect(CBBox& a) {
		// Check if any part of this bbox intersects with the bbox a
		mmax = min(maxs[0],a.maxs[0]);
		mmin = max(mins[0],a.mins[0]);
		if (mmax < mmin) return false;
		mmax = min(maxs[1],a.maxs[1]);
		mmin = max(mins[1],a.mins[1]);
		if (mmax < mmin) return false;
		mmax = min(maxs[2],a.maxs[2]);
		mmin = max(mins[2],a.mins[2]);
		if (mmax < mmin) return false;
		return true;

	}
	inline double volume() {
		return (maxs[0]-mins[0])*(maxs[1]-mins[1])*(maxs[2]-mins[2]);
	}

	double mins[3];
	double maxs[3];
};

class CTreeNode{
public:	
	unsigned char splitPlaneLeaf;
	double splitPos;
	double sumWeight;
	union {
		unsigned int* nodes;
		CTreeNode *left;
	};
	union {
		unsigned numNodes;
		CTreeNode *right;
	};

	CTreeNode() {
		splitPlaneLeaf = 0;
		splitPos = 0;
		nodes = 0;
		right = 0;
	}
	~CTreeNode() {
		if (splitPlaneLeaf & 1) {
			// Leaf
			if (nodes) DELETE_ARRAY(nodes);
			nodes = 0;
		} else {
			// Non-leaf
			if (left) DELETE(left);
			if (right) DELETE(right);
			left = right = 0;
		}
	}
};

#define CUT_OFF_NUM_NODES 10
#define MAX_DEPTH 15
#define MAX_BAD_SPLIT 3

CTreeNode* root = 0;
CBBox treebbox;
double* checkPoint;
double* nodePosI;
double* nodeWeightI;
class CompAxis{
public:
	int axis;
	CompAxis(int ax) {
		axis = ax;
	}
	bool operator() (const int &a, const int &b) {
		return nodePosI[a*3 + axis] < nodePosI[b*3 + axis];
	}
};

void pickAxis(vector<int>& nodesList, CBBox& bbox, int depth, unsigned char& axis, double& val) {
	double diff[3] = {bbox.maxs[0]-bbox.mins[0], bbox.maxs[1]-bbox.mins[1], bbox.maxs[2]-bbox.mins[2]};
	unsigned maxP = 0;
	double maxV = diff[0];

	for (int i = 1; i < 3; i++) {
		if (diff[i] > maxV) {
			maxV = diff[i];
			maxP = i;
		}
	}
	axis = maxP;
	
	// Use median
	val = bbox.mins[maxP] + 0.5*maxV;
	
	vector<int>::iterator nth = nodesList.begin() + nodesList.size()/2;
	nth_element(nodesList.begin(),nth,nodesList.end(),CompAxis(maxP));
	val = nodePosI[(*nth)*3 + axis] + 1e-6;
	
}

void buildKDNode(vector<int>& nodesList, CBBox& bbox, CTreeNode*& tnode, int depth, int badSplit ) {

	if ((nodesList.size() < CUT_OFF_NUM_NODES) || (depth > MAX_DEPTH) || (badSplit > MAX_BAD_SPLIT)) {
		// Leaf
		tnode->splitPlaneLeaf = 1;
		tnode->numNodes = nodesList.size();
		NEW_ARRAY(tnode->nodes, MYINT, tnode->numNodes);
		if (nodeWeightI) {
			double sumWeight = 0;
		
			for (int i = 0; i < tnode->numNodes; i++) {
				tnode->nodes[i] = nodesList[i];
				sumWeight += nodeWeightI[nodesList[i]];
			}
			tnode->sumWeight = sumWeight;
		} else {
			for (int i = 0; i < tnode->numNodes; i++) {
				tnode->nodes[i] = nodesList[i];
			}
			tnode->sumWeight = tnode->numNodes;
		}
	} else {
		// Non-leaf
		unsigned char axis;
		double val;
		pickAxis(nodesList, bbox, depth, axis, val);
		vector<int> leftv;
		vector<int> rightv;
		for (int i = 0; i < nodesList.size(); i++) {
			if (nodePosI[nodesList[i]*3 + axis] < val) {
				leftv.push_back(nodesList[i]);
			} else {
				if (nodePosI[nodesList[i]*3 + axis] > val) {
					rightv.push_back(nodesList[i]);
				} else {
					leftv.push_back(nodesList[i]);
					rightv.push_back(nodesList[i]);
				}
			}
		}
		tnode->splitPlaneLeaf = axis << 1;
		tnode->splitPos = val;
		NEW(tnode->left , CTreeNode);
		NEW(tnode->right , CTreeNode);
		double ov = bbox.maxs[axis];
		bbox.maxs[axis] = val;
		if (leftv.size() == nodesList.size()) {
			// Bad split
			buildKDNode(leftv, bbox, tnode->left, depth+1, badSplit+1);
		} else {
			// OK split
			buildKDNode(leftv, bbox, tnode->left, depth+1, badSplit);	
		}
		bbox.maxs[axis] = ov;
		ov = bbox.mins[axis];
		bbox.mins[axis] = val;

		if (rightv.size() == nodesList.size()) {
			// Bad split
			buildKDNode(rightv, bbox, tnode->right, depth+1, badSplit+1);
		} else {
			// OK split
			buildKDNode(rightv, bbox, tnode->right, depth+1, badSplit);	
		}

		bbox.mins[axis] = ov;

		tnode->sumWeight = tnode->left->sumWeight + tnode->right->sumWeight;
	}	
}	

double t0, t1, t2;
inline double distance2(double* a, double* b) {
	t0 = (a[0]-b[0]);
	t1 = (a[1]-b[1]);
	t2 = (a[2]-b[2]);
	return t0*t0+t1*t1+t2*t2;
}
double minDis2 = 1e50;
int nearestNodeIndex = -1;
double tempD;

double* checkNormal;
double* normalI;
bool foundAnyI = false;

inline double DotProd(double* a, double* b) {
	return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

void isThereOppositeN(CTreeNode*& tnode) {

	if (tnode->splitPlaneLeaf & 1) {
		// Leaf
		for (int i = 0; i < tnode->numNodes; i++) {
			tempD = distance2(checkPoint, &nodePosI[3*tnode->nodes[i]]);
			if (tempD < minDis2) {
				if (DotProd(checkNormal, &normalI[3*tnode->nodes[i]]) < -0.7) {
					foundAnyI = true;
					minDis2 = tempD;
				}				
			}
		}
	} else {
		// Non-leaf
		double dtoa = checkPoint[tnode->splitPlaneLeaf >> 1] - tnode->splitPos;
		if (dtoa < 0) {
			// Do left first
			isThereOppositeN(tnode->left);

			// See if we need to do right
			if (dtoa*dtoa < minDis2) {
				isThereOppositeN(tnode->right);
			}
		} else {
			// Do right firse
			isThereOppositeN(tnode->right);

			if (dtoa*dtoa < minDis2) {
				isThereOppositeN(tnode->left);
			}
		}
	}
}

inline bool isThereOppositeNormal(CTreeNode*& treeRoot, double withIn2) {
	// Find the nearest point to the checkPoint
	/* Obtain the upper bound of the nearest distance */
	minDis2 =  withIn2;
	foundAnyI = false;
	isThereOppositeN(treeRoot);
	return foundAnyI;
}


void findNearestN(CTreeNode*& tnode) {
	if (tnode->splitPlaneLeaf > 4) {
		mexErrMsgTxt("Something is wrong!\n");
	}
	if (tnode->splitPlaneLeaf & 1) {
		// Leaf
		for (int i = 0; i < tnode->numNodes; i++) {
			tempD = distance2(checkPoint, &nodePosI[3*tnode->nodes[i]]);
			if (tempD < minDis2) {
				minDis2 = tempD;
				nearestNodeIndex = tnode->nodes[i]+1;
			}
		}
	} else {
		// Non-leaf
		double dtoa = checkPoint[tnode->splitPlaneLeaf >> 1] - tnode->splitPos;
		if (dtoa < 0) {
			// Do left first
			findNearestN(tnode->left);

			// See if we need to do right
			if (dtoa*dtoa < minDis2) {
				findNearestN(tnode->right);
			}
		} else {
			// Do right firse
			findNearestN(tnode->right);

			if (dtoa*dtoa < minDis2) {
				findNearestN(tnode->left);
			}
		}
	}
}

inline int findNearestNode(MYINT guessNode = 0) {
	// Find the nearest point to the checkPoint
	/* Obtain the upper bound of the nearest distance */
	minDis2 =  distance2(&nodePosI[3*guessNode], checkPoint);
	nearestNodeIndex = guessNode+1;
	findNearestN(root);
	//mexPrintf("%d\n", nearestNodeIndex);
	return nearestNodeIndex-1;	
}
inline int findNearestNodeCustom(CTreeNode*& treeRoot, MYINT guessNode = 0) {
	// Find the nearest point to the checkPoint
	/* Obtain the upper bound of the nearest distance */
	minDis2 =  distance2(&nodePosI[3*guessNode], checkPoint);
	nearestNodeIndex = guessNode+1;
	findNearestN(treeRoot);
	//mexPrintf("%d\n", nearestNodeIndex);
	return nearestNodeIndex-1;	
}

CBBox* qbox; //Queried box 
bool anynodeinqBox(CTreeNode* tnode, CBBox& bbox) {

	if (tnode->splitPlaneLeaf & 1) {
		// A leaf, see if any node in the leave is inside the qBox
		for (int i = 0; i < tnode->numNodes; i++) {
			if (qbox->contain(&nodePosI[3*tnode->nodes[i]])) {
				return true;
			}
		}
		return false;
	} else {
		if (!bbox.intersect(*qbox)) {
			return false;
		}
		if (bbox.inside(*qbox)) {
			return true;
		}
		int axis = tnode->splitPlaneLeaf >> 1;
		double val = tnode->splitPos;
		double ov = bbox.maxs[axis];
		bbox.maxs[axis] = val;
		if (anynodeinqBox(tnode->left, bbox)) {
			bbox.maxs[axis] = ov;
			return true;
		}
		bbox.maxs[axis] = ov;

		ov = bbox.mins[axis];
		bbox.mins[axis] = val;

		if (anynodeinqBox(tnode->right, bbox)) {
			bbox.mins[axis] = ov;		
			return true;
		}
		bbox.mins[axis] = ov;		
	}
	return false;
}
double totalWeightinqBox(CTreeNode* tnode, CBBox& bbox) {
	double sum = 0;

	if (tnode->splitPlaneLeaf & 1) {
		// A leaf, see if any node in the leave is inside the qBox
		for (int i = 0; i < tnode->numNodes; i++) {
			if (qbox->contain(&nodePosI[3*tnode->nodes[i]])) {
				sum += nodeWeightI[tnode->nodes[i]];
			}
		}
	} else {
		if (!bbox.intersect(*qbox)) {
			return 0.0;
		}
		
		if (bbox.inside(*qbox)) {
			return tnode->sumWeight;
		}
		int axis = tnode->splitPlaneLeaf >> 1;
		double val = tnode->splitPos;
		double ov = bbox.maxs[axis];
		bbox.maxs[axis] = val;
		sum += totalWeightinqBox(tnode->left, bbox);

		bbox.maxs[axis] = ov;

		ov = bbox.mins[axis];
		bbox.mins[axis] = val;

		sum += totalWeightinqBox(tnode->right, bbox);
		bbox.mins[axis] = ov;		
	}
	return sum;
}

vector<int> pointsInqBox;
void getPointsInqBox(CTreeNode* tnode, CBBox& bbox, bool allIn) {
	if (!allIn) {
		if (tnode->splitPlaneLeaf & 1) {
			// A leaf, see if any node in the leave is inside the qBox
			for (int i = 0; i < tnode->numNodes; i++) {
				if (qbox->contain(&nodePosI[3*tnode->nodes[i]])) {
					pointsInqBox.push_back(tnode->nodes[i]);
				}
			}
		} else {
			if (!bbox.intersect(*qbox)) {
				return;
			}
			
			if (bbox.inside(*qbox)) {
				// Everything within this subtree is inside
				getPointsInqBox(tnode->left, bbox, true);
				getPointsInqBox(tnode->right, bbox, true);
				
			} else {
				int axis = tnode->splitPlaneLeaf >> 1;
				double val = tnode->splitPos;
				double ov = bbox.maxs[axis];
				bbox.maxs[axis] = val;
				getPointsInqBox(tnode->left, bbox, false);

				bbox.maxs[axis] = ov;

				ov = bbox.mins[axis];
				bbox.mins[axis] = val;

				getPointsInqBox(tnode->right, bbox, false);		
				bbox.mins[axis] = ov;	
			}
		}
	} else {
		// All in
		if (tnode->splitPlaneLeaf & 1) {
			for (int i = 0; i < tnode->numNodes; i++) {
				pointsInqBox.push_back(tnode->nodes[i]);
			}				
		} else {
			getPointsInqBox(tnode->left, bbox, true);
			getPointsInqBox(tnode->right, bbox, true);
		}
	}
}
void getPointsInqBoxStrictly(CTreeNode* tnode, CBBox& bbox, bool allIn) {
	if (!allIn) {
		if (tnode->splitPlaneLeaf & 1) {
			// A leaf, see if any node in the leave is inside the qBox
			for (int i = 0; i < tnode->numNodes; i++) {
				if (qbox->containStrictly(&nodePosI[3*tnode->nodes[i]])) {
					pointsInqBox.push_back(tnode->nodes[i]);
				}
			}
		} else {
			if (!bbox.intersect(*qbox)) {
				return;
			}
			
			if (bbox.insideStrictly(*qbox)) {
				// Everything within this subtree is inside
				getPointsInqBoxStrictly(tnode->left, bbox, true);
				getPointsInqBoxStrictly(tnode->right, bbox, true);
				
			} else {
				int axis = tnode->splitPlaneLeaf >> 1;
				double val = tnode->splitPos;
				double ov = bbox.maxs[axis];
				bbox.maxs[axis] = val;
				getPointsInqBoxStrictly(tnode->left, bbox, false);

				bbox.maxs[axis] = ov;

				ov = bbox.mins[axis];
				bbox.mins[axis] = val;

				getPointsInqBoxStrictly(tnode->right, bbox, false);		
				bbox.mins[axis] = ov;	
			}
		}
	} else {
		// All in
		if (tnode->splitPlaneLeaf & 1) {
			for (int i = 0; i < tnode->numNodes; i++) {
				pointsInqBox.push_back(tnode->nodes[i]);
			}				
		} else {
			getPointsInqBoxStrictly(tnode->left, bbox, true);
			getPointsInqBoxStrictly(tnode->right, bbox, true);
		}
	}
}
void buildTree(double* nodePos, int numPos, vector<int>* considerNode = NULL) {
	int i;
	vector<int> nodesList;
	double mins[3]={1e50, 1e50, 1e50}, maxs[3]={-1e50, -1e50, -1e50};
	double* np = nodePos;
	for (i = 0; i < numPos; i++) {
		nodesList.push_back(i);

		if (np[0] < mins[0]) mins[0] = np[0];
		if (np[1] < mins[1]) mins[1] = np[1];
		if (np[2] < mins[2]) mins[2] = np[2];

		if (np[0] > maxs[0]) maxs[0] = np[0];
		if (np[1] > maxs[1]) maxs[1] = np[1];
		if (np[2] > maxs[2]) maxs[2] = np[2];

		np += 3;
//		mexPrintf("%d %f %f %f, %f %f %f\n",i,minx,miny,minz,maxx,maxy,maxz);
	}
	if (root) DELETE(root);
	memcpy(&treebbox.mins, &mins, sizeof(double)*3);
	memcpy(&treebbox.maxs, &maxs, sizeof(double)*3);

	NEW( root,  CTreeNode);
	nodeWeightI = 0;
	nodePosI = nodePos;
	if (considerNode) {
		buildKDNode(*considerNode, treebbox, root, 0, 0);
	} else {
		buildKDNode(nodesList, treebbox, root, 0, 0);
	}
}
void buildTreeCustom(CTreeNode*& croot, CBBox& ctreebbox, double* nodePos, int numPos, vector<int>* considerNode = NULL) {

	int i;
	vector<int> nodesList;
	double mins[3]={1e50, 1e50, 1e50}, maxs[3]={-1e50, -1e50, -1e50};
	double* np = nodePos;
	for (i = 0; i < numPos; i++) {
		nodesList.push_back(i);

		if (np[0] < mins[0]) mins[0] = np[0];
		if (np[1] < mins[1]) mins[1] = np[1];
		if (np[2] < mins[2]) mins[2] = np[2];

		if (np[0] > maxs[0]) maxs[0] = np[0];
		if (np[1] > maxs[1]) maxs[1] = np[1];
		if (np[2] > maxs[2]) maxs[2] = np[2];

		np += 3;
//		mexPrintf("%d %f %f %f, %f %f %f\n",i,minx,miny,minz,maxx,maxy,maxz);
	}
	if (croot) DELETE(croot);
	memcpy(&ctreebbox.mins, &mins, sizeof(double)*3);
	memcpy(&ctreebbox.maxs, &maxs, sizeof(double)*3);

	NEW(croot, CTreeNode);
	nodeWeightI = 0;
	nodePosI = nodePos;
	if (considerNode) {
		buildKDNode(*considerNode, treebbox, croot, 0, 0);
	} else {
		buildKDNode(nodesList, treebbox, croot, 0, 0);
	}
}
void buildTreeCustomWithWeight(CTreeNode*& croot, CBBox& ctreebbox, double* nodePos, double* nodeWeight, int numPos, vector<int>* considerNode = NULL) {

	int i;
	vector<int> nodesList;
	double mins[3]={1e50, 1e50, 1e50}, maxs[3]={-1e50, -1e50, -1e50};
	double* np = nodePos;
	for (i = 0; i < numPos; i++) {
		nodesList.push_back(i);

		if (np[0] < mins[0]) mins[0] = np[0];
		if (np[1] < mins[1]) mins[1] = np[1];
		if (np[2] < mins[2]) mins[2] = np[2];

		if (np[0] > maxs[0]) maxs[0] = np[0];
		if (np[1] > maxs[1]) maxs[1] = np[1];
		if (np[2] > maxs[2]) maxs[2] = np[2];

		np += 3;
//		mexPrintf("%d %f %f %f, %f %f %f\n",i,minx,miny,minz,maxx,maxy,maxz);
	}
	if (croot) DELETE(croot);
	memcpy(&ctreebbox.mins, &mins, sizeof(double)*3);
	memcpy(&ctreebbox.maxs, &maxs, sizeof(double)*3);

	NEW(croot, CTreeNode);
	nodeWeightI = nodeWeight;
	nodePosI = nodePos;
	if (considerNode) {
		buildKDNode(*considerNode, treebbox, croot, 0, 0);
	} else {
		buildKDNode(nodesList, treebbox, croot, 0, 0);
	}
}
