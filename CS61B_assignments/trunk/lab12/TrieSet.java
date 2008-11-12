/** A set of strings. */
public class TrieSet {
	
	/** An empty set. */
	public TrieSet () {
		root = new EmptyTrieNode ();
	}
	
	/** True iff THIS contains KEY. */
	public boolean contains (String key) {
		return root.contains (key.toLowerCase(), 0);
	}
	
	/** Add KEY to THIS.  Has no effect if KEY is already in THIS. */
	void add (String key) {
		root = root.insert (key.toLowerCase(), 0);
	}
	
	private TrieNode root;
	
	private static abstract class TrieNode {
		
		/** True iff THIS contains KEY, assuming that THIS is a TrieNode
		 *  at level K in the entire tree (where the root is at level 0). 
		 *  Assumes that the length of KEY is at least K, and that THIS is
		 *  the subtree that should contain KEY, if present. */
		abstract boolean contains (String key, int k);
		
		/** Insert KEY into THIS, if necesary, assuming that THIS is a TrieNode 
		 *  at level K in the entire tree (where the root is at level 0)
		 *  Assumes that the length of KEY is at least K, and that THIS is
		 *  the subtree that should contain KEY, if present. */
		abstract TrieNode insert (String key, int k);
	}
	
	/** Represents an empty trie. */
	private static class EmptyTrieNode extends TrieNode {
		boolean contains (String key, int k) {
			return false;
		}
		
		TrieNode insert (String key, int k) {
			return new SingletonTrieNode(key);
		}
	}
	
	
	/** Represents a trie subtree that contains a single String. */
	private static class SingletonTrieNode extends TrieNode {
		final String key;
		SingletonTrieNode (String key) {
			this.key = key;
		}
		
		boolean contains (String key, int k) {
			if (key.equals(this.key)) return true;
			else return false;
		}
		
		TrieNode insert (String key, int k) {
			if (key.equals(this.key)) { 
				return this;
			} else {
				TrieNode newNode = new InnerTrieNode(this, k);
				newNode = newNode.insert(key, k);
				return newNode;
			}
		}
	}
	
	
	/** A nonleaf node in a trie. */
	private static class InnerTrieNode extends TrieNode {
		/** An inner node at level K (root is 0) that initially contains just 
		 *  the string contained in S. [It is convenient to do it this way
		 *  because one always creates an inner node out of a singleton node, 
		 *  and the same singleton node moves down the tree.] */
		InnerTrieNode (SingletonTrieNode s, int k) {
			children = new TrieNode[27];
			for (int i = 0; i < children.length; i++) {
				children[i] = new EmptyTrieNode();
			}
			children[(int) s.key.charAt(k) - 97] = s;
			this.key = s.key.substring(0, k);
		}
		
		boolean contains (String key, int k) {
			if (key.startsWith(this.key)) {
				return children[(int) key.charAt(k) - 97].contains(key, k + 1);
			}
			return false;
		}
			
			TrieNode insert (String key, int k) {
				children[(int) key.charAt(k) - 97] = children[(int) key.charAt(k) - 97].insert(key, k + 1);
				return this;
			}
			
			String key;
			private TrieNode[] children;
		}
		
	}
