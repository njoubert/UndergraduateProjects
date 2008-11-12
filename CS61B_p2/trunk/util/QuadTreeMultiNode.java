package util;

abstract class QuadTreeMultiNode extends QuadTreeNode {
	
	QuadTreeMultiNode(QuadTree wholeTree, QuadTreeNode parent, int quadrant) {
		super(wholeTree, parent, quadrant);
	} 
	
	/** Insert the object with unique id ID into THIS, returning the 
	 *  resulting tree.  Throws IllegalArgumentException if ID does not
	 *  represent a particle that is within the bounding box of THIS. */
	QuadTreeNode insert (int id) {
		for (int i = 0; i < 4; i++) {
			if (children[i].isWithin(id)) {
				children[i] = children[i].insert(id);
				break;
			}
		}
		return this;
	}
	
	QuadTreeNode remove (int id) {
		for (int i = 0; i < 4; i++) {
			if (children[i].isWithin(id)) {
				children[i] = children[i].remove(id);
				break;
			}
		}
		return justifyExistence();
	}
	
	/**
	 * To ensure that we don't have a long structure of inner nodes 
	 * ending in just one leaf node, any multinode needs to justify
	 * its existence by making sure it does not have just one leaf
	 * node as its child, and if it has, it needs to destroy itself
	 * and put its only LeafNode in its place.
	 * @return this if justified, new leafnode containing its child's ID if not justified.
	 */
	abstract QuadTreeNode justifyExistence();
	
	protected QuadTreeNode children[] = new QuadTreeNode[4];
	
}
