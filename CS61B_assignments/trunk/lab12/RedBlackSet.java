import java.io.PrintStream;
import java.util.ArrayList;

/** Represents a set of objects of type T. */
public class RedBlackSet<T extends Comparable<? super T>> {
	
	/** An empty set. */
	public RedBlackSet () {
		root = null;
	}
	
	/** True iff THIS contains the KEY */
	public boolean contains (T key) {
		return contains (root, key);
	}
	
	/** True iff TREE contains KEY. */
	private boolean contains (RedBlackNode<T> tree, T key) {
		while (tree != null) {
			int cmp = tree.key.compareTo (key);
			if (cmp < 0)
				tree = tree.right;
			else if (cmp > 0)
				tree = tree.left;
			else
				return true;
		}
		return false;
	}
	
	/** Add KEY to THIS.   Does nothing if KEY is already present. */
	public void add (T key) {
		root = RedBlackNode.insert (root, key);
	}
	
	/** Print the contents of THIS to OUT in the form of a preorder 
	 *  traveral of the corresponding 2-4 tree.  For each node of the
	 *  2-4 tree, print one line containing its keys in order, separated 
	 *  by single spaces and indented by 3 spaces for each level it is
	 *  below the root of the 2-4 tree. */
	public void dump (PrintStream out) {
		dump (out, root, 0);
	}
	
	/** Print the contents of TREE to OUT in the form of a preorder 
	 *  traveral of the corresponding 2-4 tree.  For each node of the
	 *  2-4 tree, print one line containing its keys in order, separated 
	 *  by single spaces and indented by INDENT + 3 spaces for each level 
	 *  it is below the root of the 2-4 tree. */
	private void dump (PrintStream out, RedBlackNode<T> tree, int indent) {
		ArrayList<RedBlackNode<T>> queue = new ArrayList();
		
		for (int i = 0; i < indent; i++) {
			out.print(" ");
		}
		
		if (tree.left != null && tree.left.isRed) {
			out.print(tree.left.key + " ");
			queue.add(tree.left.left);
			queue.add(tree.left.right);
		} else {
			queue.add(tree.left);
		}
		
		out.print(tree.key);
		
		if (tree.right != null && tree.right.isRed) {
			out.print(" " + tree.right.key);
			queue.add(tree.right.left);
			queue.add(tree.right.right);
		} else {
			queue.add(tree.right);
		}
		
		for (RedBlackNode<T> nodes : queue) {
			dump (out, nodes, indent + 3);
		}
		
	}
	
	private RedBlackNode<T> root;
	
	/** A node of a red-black tree.  A leaf node is represented by null. */
	private static class RedBlackNode<T extends Comparable<? super T>> {
		final T key;
		/** True iff THIS is currently a red node. */
		boolean isRed;
		RedBlackNode<T> left, right, parent;
		
		/** A new red node with given KEY, LEFT and RIGHT children, and 
		 *  PARENT. */
		RedBlackNode (T key, RedBlackNode<T> left, RedBlackNode<T> right,
				RedBlackNode<T> parent) {
			isRed = true;
			this.key = key;
			this.left = left; this.right = right; this.parent = parent;
		}
		
		/** The sibling of THIS, assuming that THIS is not the root of
		 *  the entire tree. */
		RedBlackNode<T> sibling () {
			if (this == parent.right)
				return parent.left;
			else 
				return parent.right;
		}
		
		/** Rotate tree left around THIS.  That is, the right child of THIS
		 *  becomes the new root of the subtree.  [See page 178 of
		 *  Data Structures (Into Java).] */
		void rotateLeft () {
			//Grab right node = D
			RedBlackNode<T> D = this.right;
			
			//D's right becomes this' right, swop parents
			this.right = D.right;
			D.right.parent = this;
			
			//D's left becomes D's right
			D.right = D.left;
			
			//this' left becomes D's left, swop parents
			D.left = this.left;
			D.left.parent = this;
			
			//D becomes this' left
			this.left = D;
			
			//Swop D and This' keys
			T tempKey = this.key;
			this.key = D.key;
			D.key = tempKey;
			
		}
		
		/** Rotate tree right around THIS.  That is, the left child of THIS
		 *  becomes the new root of the subtree.  [See page 178 of
		 *  Data Structures (Into Java).] */
		void rotateRight () {
			//Grab left node = B
			RedBlackNode<T> B = this.left;
			
			//B's left child becomes D's left child
			this.left = B.left;
			B.left.parent = this;
			
			//B's right child becomes B's left child/
			B.left = B.right;
			
			//D's right child becomes B's right child
			B.right = this.right;
			this.right.parent = B;
			
			//B becomes right child of D;
			this.right = B;
			
			//Swop B and D's keys
			T tempKey = this.key;
			this.key = B.key;
			B.key = tempKey;
		}
		
		/** Destructively insert KEY into TREE, returning the modified tree,
		 *  and maintaining the red-black property. Does nothing if
		 *  KEY is already present in the tree. */
		static <T extends Comparable<? super T>> RedBlackNode<T> insert (RedBlackNode<T> tree, T key) {
			// REPLACE WITH SOLUTION
			return null;
		}
		
	}    
}
