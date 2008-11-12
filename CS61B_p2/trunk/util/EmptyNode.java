package util;

/**
 * Represents a node with nothing in it. Works as base case for all procedures.
 * @author Niels Joubert cs61b-bo
 *
 */
public class EmptyNode extends QuadTreeNode {

	  EmptyNode (QuadTree wholeTree, QuadTreeNode parent, int quadrant) {
	    super(wholeTree, parent, quadrant);
	  }

	  /** Inserting into an EmptyNode changes it to a LeafNode
	   * */
	  QuadTreeNode insert (int id) {
		  return new LeafNode(myTree, myParent, quadrant, id);
	  }
	  
	  /**
	   * Removing an EmptyNode inherently has no action
	   */
	  QuadTreeNode remove (int id) {
		  return this;
	  }
	  
	  String type () { return "empty"; }
	  
}
