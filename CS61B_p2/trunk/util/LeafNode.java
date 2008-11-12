package util;

public class LeafNode extends QuadTreeNode {
	
	LeafNode (QuadTree wholeTree, QuadTreeNode parent, int quadrant, int id) {
		super(wholeTree, parent, quadrant);
		this.id = id;
	}
	
	/**
	 * Insertion into  leaf node means changing it into an InnerNode and inserting both 
	 * the new ID and the ID of the leafnode you replaced.
	 */
	QuadTreeNode insert (int id) {
		return (new InnerNode(myTree, myParent, quadrant)).insert(this.id).insert(id);
	}
	
	/**
	 * Removal of a LeafNode means replacing it with an EmptyTree
	 */
	QuadTreeNode remove (int id) {
		if (id == this.id) return new EmptyNode(this.myTree, this.myParent, quadrant);
		else return this;
	}
	
	String type () { return "leaf"; }
	
	final int id;
	
}
