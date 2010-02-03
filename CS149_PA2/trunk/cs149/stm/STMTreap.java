package cs149.stm;

import cs149.stm.CoarseLockTreap.Node;

public class STMTreap implements IntSet {
	
	/**
	 * Single Node in Treap
	 */
    static class Node {
        final int key;
        final int priority;
        Node left;
        Node right;

        Node(final int key, final int priority) {
            this.key = key;
            this.priority = priority;
        }

        public String toString() {
            return "Node[key=" + key + ", prio=" + priority +
                    ", left=" + (left == null ? "null" : String.valueOf(left.key)) +
                    ", right=" + (right == null ? "null" : String.valueOf(right.key)) + "]";
        }
    }

    private long randState = 0;
    private Node root;

    @org.deuce.Atomic public boolean contains(final int key) {
        Node node = root;
        while (node != null) {
            if (key == node.key) {
                return true;
            }
            node = key < node.key ? node.left : node.right;
        }
        return false;
    }

    
    @org.deuce.Atomic public void add(final int key) {
    	//This is an issue. By setting the root to a value, 
    	//we will abort all read instructions somewhere in it. 
    	//Really we want to search down until we find where to modify, then modify.
    	addImpl(root, null, key);
    }

    private void addImpl(final Node node, final Node parent, final int key) {
        if (node == null) {
            root = new Node(key, randPriority());
        } else if (key == node.key) {
            return;// no insert needed
        } else if (key < node.key) {
        	if (node.left == null) {
        		node.left = new Node(key, randPriority());
        	} else {
	            addImpl(node.left, node, key);
        	}
        	if (node.left.priority > node.priority) {
                rotateRight(node, parent);
            }
        }
        else {
        	if (node.right == null)
        		node.right = new Node(key, randPriority());
        	else
        		addImpl(node.right, node, key);
            if (node.right.priority > node.priority) {
                rotateLeft(node, parent);
            }
        }
    }

    private int randPriority() {
        // The constants in this 64-bit linear congruential random number
        // generator are from http://nuclear.llnl.gov/CNP/rng/rngman/node4.html
        randState = randState * 2862933555777941757L + 3037000493L;
        return (int)(randState >> 30);
    }

    private void rotateRight(final Node node, final Node parent) {
        //       node                  nL
        //     /      \             /      \
        //    nL       z     ==>   x       node
        //  /   \                         /   \
        // x   nLR                      nLR   z
        final Node nL = node.left;
        node.left = nL.right;
        nL.right = node;
        if (parent == null)
        	root = nL;
        else if (parent.left.equals(node))
        	parent.left = nL;
        else
        	parent.right = nL;
    }

    private void rotateLeft(final Node node, final Node parent) {
        final Node nR = node.right;
        node.right = nR.left;
        nR.left = node;
        if (parent == null)
        	root = nR;
        else if (parent.right.equals(node))
        	parent.right = nR;
        else
        	parent.left = nR;
    }
    
    private Node rotateRight(final Node node) {
        final Node nL = node.left;
        node.left = nL.right;
        nL.right = node;
        return nL;
    }

    private Node rotateLeft(final Node node) {
        final Node nR = node.right;
        node.right = nR.left;
        nR.left = node;
        return nR;
    }

    @org.deuce.Atomic public void remove(final int key) {
    	//Same problem as with add, this sets the root to something. We want to first search to where we should set a value, then start setting it.
        removeImpl(root, null, key);
    }

    private Node removeImpl(final Node node, final Node parent, final int key) {
        if (node == null) {
            root = new Node(key, randPriority());
        }
        else if (key == node.key) { //We have found what we want to remove.
            if (node.left == null) {
            	if (parent == null)
            		root = node.right;
            	else if (parent.left.equals(node))
            		parent.left = node.right;
            	else
            		parent.right = node.right;
            }
            else if (node.right == null) {
            	if (parent == null)
            		root = node.left;
            	else if (parent.left.equals(node))
            		parent.left = node.left;
            	else
            		parent.right = node.left;
            } else {
                // Two children, this is the hardest case.  We will pretend
                // that node has -infinite priority, move it down, then retry
                // the removal.
                if (node.left.priority > node.right.priority) {
                    // node.left needs to end up on top
                    final Node top = rotateRight(node);
                    top.right = removeImpl(top.right, key);
                    return top;
                } else {
                    final Node top = rotateLeft(node);
                    top.left = removeImpl(top.left, key);
                    return top;
                }
            }
        }
        else if (key < node.key) {
            removeImpl(node.left, node, key);
        }
        else {
            removeImpl(node.right, node, key);
        }
    }
}
