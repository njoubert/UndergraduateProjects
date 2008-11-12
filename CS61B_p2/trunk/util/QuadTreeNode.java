package util;

/** Represents a single node in a quad tree.  This is abstract in case
 *  you want to have different kinds of QuadTreeNode corresponding to
 *  empty trees, leaf trees (no children, just objects), and internal
 *  trees (those with children). */
abstract class QuadTreeNode {
	
	/** The QuadTree I am part of. */
	final QuadTree myTree;
	
	/** The QuadTreeNode, if any, that contains me. */
	QuadTreeNode myParent;
	
	/** A new QuadTreeNode that is part of WHOLETREE, in the quadrant QUADRANT of its parent */ 
	QuadTreeNode (QuadTree wholeTree, QuadTreeNode parent, int quadrant) {
		myTree = wholeTree; myParent = parent; this.quadrant = quadrant;
	}
	
	
	/** My smallest X coordinate. */
	double llx () {
		if (quadrant == 0) return myParent.midPX();
		if (quadrant == 1) return myParent.llx();
		if (quadrant == 2) return myParent.llx();
		return myParent.midPX();
	}
	
	/** My smallest Y coordinate. */
	double lly () {
		if (quadrant == 0) return myParent.midPY();
		if (quadrant == 1) return myParent.midPY();
		if (quadrant == 2) return myParent.lly();
		return myParent.lly();
	}
	
	/** My largest X coordinate */
	double urx () {
		if (quadrant == 0) return myParent.urx();
		if (quadrant == 1) return myParent.midPX();
		if (quadrant == 2) return myParent.midPX();
		return myParent.urx();
	}
	
	/** My largest Y coordinate */
	double ury () {
		if (quadrant == 0) return myParent.ury();
		if (quadrant == 1) return myParent.ury();
		if (quadrant == 2) return myParent.midPY();
		return myParent.midPY();
	}
	
	double midPX() { return 0.5*llx() + 0.5*urx();}
	double midPY() { return 0.5*lly() + 0.5*ury();}	
	
	/** True iff the particle with unique id P is within my bounding box. */
	boolean isWithin (int p) {
		return isWithin(p, llx(), lly(), urx(), ury());
	}
	
	boolean isWithin(int p, double llx, double lly, double urx, double ury) {
		return 
		myTree.x (p) >= llx && myTree.x (p) < urx &&
		myTree.y (p) >= lly && myTree.y (p) < ury; 
	}
	
	/** Insert the object with unique id ID into THIS, returning the 
	 *  resulting tree.  Throws IllegalArgumentException if ID does not
	 *  represent a particle that is within the bounding box of THIS. */
	abstract QuadTreeNode insert (int id);
	
	abstract QuadTreeNode remove (int id);
	
	abstract String type ();
	
	int quadrant;
	
}

