package util;

/**
 * This is a node that represents 4 quadrants. The 4 quadrants
 * are 
 * @author Niels Joubert cs61b-bo
 *
 */
public class InnerNode extends QuadTreeMultiNode {
	
	/**
	 * An array of my children, indexed by Quadrant number, from upper right, counterclocwise to lower right.
	 */
	
	
	/** A new QuadTreeNode that is part of WHOLETREE. */ 
	InnerNode (QuadTree wholeTree, QuadTreeNode parent, int quadrant) {
		super(wholeTree, parent, quadrant);
		llx = super.llx();
		lly = super.lly();
		urx = super.urx();
		ury = super.ury();
		children[0] = new EmptyNode(this.myTree, this, 0);
		children[1] = new EmptyNode(this.myTree, this, 1);
		children[2] = new EmptyNode(this.myTree, this, 2);
		children[3] = new EmptyNode(this.myTree, this, 3);
	}
	
	double llx () { return llx; }
	double lly () { return lly; }
	double urx () { return urx; }
	double ury () { return ury; }
	
	String type () { return "inner"; }
	
	/**
	 *  counts the number of EmptyNode children and finds
	 *	the only LeafNode (if there is only one)
	 *	and will destructively change its only LeafNode
	 *	to take its place if there is only one LeafNode and 
	 *	three EmptyNodes as children.
	 */
	QuadTreeNode justifyExistence() {
		int emptyCount = 0;
		QuadTreeNode lastLeaf = null;
		for (int i = 0; i < 4; i++) {
			if (children[i].type().equals("empty")) {
				emptyCount += 1;
			} else if (children[i].type().equals("leaf")){
				lastLeaf = children[i];
			}
		}
		if (emptyCount == 3 && lastLeaf != null) {
			lastLeaf.myParent = myParent;
			lastLeaf.quadrant = quadrant;
			return lastLeaf;
		} else {
			return this;
		}
		
	}
	
	
	double llx, lly, urx, ury;
	
}
