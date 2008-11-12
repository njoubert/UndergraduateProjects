package util;

public class RootNode extends QuadTreeMultiNode {
	
	RootNode(QuadTree myTree) {
		super(myTree, null, 0);
		children[0] = new EmptyNode(this.myTree, this, 0);
		children[1] = new EmptyNode(this.myTree, this, 1);
		children[2] = new EmptyNode(this.myTree, this, 2);
		children[3] = new EmptyNode(this.myTree, this, 3);
	}
	
	double llx () { return myTree.llx(); }
	double lly () { return myTree.lly(); }
	double urx () { return myTree.urx(); }
	double ury () { return myTree.ury(); }
	

	/** A root node will ALWAYS justify its existence, because it is the 
	 * Master of the House, the Keeper of the Keys, and the Order Creator
	 * of the Quadtree.
	 */
	QuadTreeNode justifyExistence() {
		return this;
	}
	
	String type() { return "root"; }

	
}
