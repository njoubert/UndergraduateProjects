package cs149.stm;

import java.util.concurrent.atomic.AtomicLong;

public class STMTreap implements IntSet {
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

    private AtomicLong randState = new AtomicLong(0);
    private Node root;

    public @org.deuce.Atomic boolean contains(final int key) {
        Node node = root;
        while (node != null) {
            if (key == node.key) {
                return true;
            }
            node = key < node.key ? node.left : node.right;
        }
        return false;
    }

    public @org.deuce.Atomic void add(final int key) {
        Node temp = addImpl(root, key);
        if (root != temp)
        	root = temp;
    }

    private Node addImpl(final Node node, final int key) {
        if (node == null) {
            return new Node(key, randPriority());
        }
        else if (key == node.key) {
            // no insert needed
            return node;
        }
        else if (key < node.key) {
        	Node temp = addImpl(node.left, key);
        	if (temp != node.left)
        		node.left = temp;
            if (node.left.priority > node.priority) {
                return rotateRight(node);
            }
            return node;
        }
        else {
            Node temp = addImpl(node.right, key);
            if (temp != node.right)
            	node.right = temp;
            if (node.right.priority > node.priority) {
                return rotateLeft(node);
            }
            return node;
        }
    }

    private int randPriority() {
        // The constants in this 64-bit linear congruential random number
        // generator are from http://nuclear.llnl.gov/CNP/rng/rngman/node4.html
    	long state = randState.get();
        while(!randState.compareAndSet(state, state * 2862933555777941757L + 3037000493L)) {
        	state = randState.get();
        }
        return (int)((state * 2862933555777941757L + 3037000493L) >> 30);
    }

    private Node rotateRight(final Node node) {
        //       node                  nL
        //     /      \             /      \
        //    nL       z     ==>   x       node
        //  /   \                         /   \
        // x   nLR                      nLR   z
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

    public @org.deuce.Atomic void remove(final int key) {
        Node temp = removeImpl(root, key);
        if (temp != root)
        	root = temp;
    }

    private Node removeImpl(final Node node, final int key) {
        if (node == null) {
            // not present, nothing to do
            return null;
        }
        else if (key == node.key) {
            if (node.left == null) {
                // splice out this node
                return node.right;
            }
            else if (node.right == null) {
                return node.left;
            }
            else {
                // Two children, this is the hardest case.  We will pretend
                // that node has -infinite priority, move it down, then retry
                // the removal.
                if (node.left.priority > node.right.priority) {
                    // node.left needs to end up on top
                    final Node top = rotateRight(node);
                    Node temp = removeImpl(top.right, key);
                    if (temp != top.right)
                    	top.right = temp;
                    return top;
                } else {
                    final Node top = rotateLeft(node);
                    Node temp = removeImpl(top.left, key);
                    if (temp != top.left)
                    	top.left = temp;
                    return top;
                }
            }
        }
        else if (key < node.key) {
        	Node temp = removeImpl(node.left, key);
        	if (temp != node.left)
        		node.left = temp;
            return node;
        }
        else {
            Node temp = removeImpl(node.right, key);
            if (temp != node.right)
            	node.right = temp;
            return node;
        }
    }
}
