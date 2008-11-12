import java.util.*;

public class BinaryTree<Item> {

  private TreeNode<Item> myRoot;
	
  public BinaryTree () {
    myRoot = null;
  }
	
  public BinaryTree (TreeNode<Item> t) {
    myRoot = t;
  }
	
  /** Print the values in the tree in preorder: root value first,
   *  then values in the left subtree (in preorder), then values
   *  in the right subtree (in preorder). */
  public void printPreorder ( ) {
    printPreorder (myRoot);
    System.out.println ( );
  }

  // We'll discuss the peculiar syntax below in a future lecture.  The
  // type parameter Item must be passed in as a "parameter" to each
  // static method, and this is how you do that.

  private static <Item> void printPreorder (TreeNode<Item> t) {
    if (t != null) {
      System.out.print (t.myItem + " ");
      printPreorder (t.left);
      printPreorder (t.right);
    }
  }

  /** Print the values in the tree in inorder: values in the left
   *  subtree first (in inorder), then the root value, then values
   *  in the right subtree (in inorder). */
  public void printInorder ( ) {
    printInorder (myRoot);
    System.out.println ( );
  }

  private static <Item> void printInorder (TreeNode<Item> t) {
    if (t != null) {
      printInorder (t.left);
      System.out.print (t.myItem + " ");
      printInorder (t.right);
    }
  }
	
  public static BinaryTree<String> sampleTree1 ( ) {
    return new BinaryTree<String> 
      (new TreeNode<String> ("a",
			     new TreeNode<String> ("b"),
			     new TreeNode<String> ("c")));
  }

  public static BinaryTree<String> sampleTree2 ( ) {
    return new BinaryTree<String> 
      (new TreeNode<String> 
       ("a",
	new TreeNode<String> ("b",
			      new TreeNode<String> ("d",
						    new TreeNode<String> ("e"),
						    new TreeNode<String> ("f")),
			      null),
	new TreeNode<String> ("c")));
  }
  
  public static BinaryTree<String> sampleTree3 () {
	  return new BinaryTree<String>
	  (new TreeNode<String> ("a", new TreeNode<String> ("b"), new TreeNode<String> ("c", new TreeNode<String> ("d", new TreeNode<String> ("e"), new TreeNode<String> ("f")), null)));
  }
  
  public static BinaryTree<String> balancedTree() {
	  return new BinaryTree<String>
	  (new TreeNode<String> ("a", new TreeNode<String> ("b", new TreeNode<String> ("d", new TreeNode<String> ("h") ,new TreeNode<String> ("i")), new TreeNode<String> ("e", new TreeNode<String> ("j"),new TreeNode<String> ("k") )), new TreeNode<String> ("c", new TreeNode<String>("f", new TreeNode<String> ("l"),new TreeNode<String> ("m") ) , new TreeNode<String> ("g",new TreeNode<String> ("n") ,new TreeNode<String> ("o") ))));
  }

  public static void main (String [ ] args) {
    print (new BinaryTree<String> ( ), "the empty tree");
    print (sampleTree1 ( ), "sample tree 1");
    print (sampleTree2 ( ), "sample tree 2");
    print (sampleTree3 ( ), "sample tree 3");
    print (balancedTree ( ), "balanced tree ");
    System.out.println (sampleTree1().height());
    System.out.println (sampleTree2().height());
    System.out.println (sampleTree3().height());
    System.out.println (balancedTree().height());
    System.out.println("tree1 is: " + sampleTree1().isCompletelyBalanced());
    System.out.println("tree2 is: " + sampleTree2().isCompletelyBalanced());
    System.out.println("tree3 is: " + sampleTree3().isCompletelyBalanced());
    System.out.println("balancedtree is: " + balancedTree().isCompletelyBalanced());
  }

  private static <Item> void print (BinaryTree<Item> t, String description) {
    System.out.println (description + " in preorder");
    t.printPreorder ( );
    System.out.println (description + " in inorder");
    t.printInorder ( );
    System.out.println ( );
  }

  /** Height of a given binaryTree - the maximum amount of nodes between the root and the leaf */
  public int height() {
	  return height(myRoot);
  }
  
  private static int height(TreeNode a) {
	  if (a == null) return -1;
	   else return 1 + Math.max(height(a.left), height(a.right));
  }
  
  /** Checks whether the tree is completely balanced
   * which means:
   * the height of the left child equals the height of the right child
   * and both these children are also balanced;
   * An empty tree and a tree containing just one node are both completely balanced;
   */
  public boolean isCompletelyBalanced() {
	  return isCompletelyBalanced(myRoot);
  }
  
  private static boolean isCompletelyBalanced(TreeNode a) {
	  if (a == null || (a.left == null && a.right == null)) {
		  return true;
	  } else {
		  if (height(a.left) == height(a.right)) {
			  if (isCompletelyBalanced(a.left) && isCompletelyBalanced(a.right)) {
				  return true;
			  } else return false;
		  } else {
			  return false;
		  }
	  }
  }
  static class TreeNode<Item> {
		
    public Item myItem;
    public TreeNode<Item> left;
    public TreeNode<Item> right;
		
    public TreeNode (Item obj) {
      myItem = obj;
      left = right = null;
    }
		
    public TreeNode (Item obj, TreeNode<Item> left, TreeNode<Item> right) {
      myItem = obj;
      this.left = left;
      this.right = right;
    }
  }

  /** Iterates through all my TreeNodes in (depth-first) preorder. */
  // See lab section 9.3.1.
  public class DepthFirstIterator implements Iterator<TreeNode<Item>> {

    private Stack<TreeNode<Item>> fringe = new Stack<TreeNode<Item>> ( );

    public DepthFirstIterator ( ) {
      // Java fact.  Since DepthFirstIterator is an inner class of
      // BinaryTree, and has no "myRoot" member itself, 
      // "myRoot" here is shorthand for "BinaryTree.this.myRoot".
      if (myRoot != null) {
	fringe.push (myRoot);
      }
    }

    public boolean hasNext ( ) {
      return !fringe.empty ( );
    }

    public TreeNode<Item> next ( ) {
      if (!hasNext ( )) {
	throw new NoSuchElementException ("tree ran out of elements");
      }
      TreeNode<Item> node = fringe.pop ( );
      if (node.right != null) {
	fringe.push (node.right);
      }
      if (node.left != null) {
	fringe.push (node.left);
      }
      return node;
    }

    public void remove () {
        throw new UnsupportedOperationException ();
    }

  }
  
  /** Iterates through all my TreeNodes in InOrder. */
  // See lab section 9.3.1.

}

