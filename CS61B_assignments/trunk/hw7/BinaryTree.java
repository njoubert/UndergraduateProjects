import java.util.*;

public class BinaryTree<Item> {

  private TreeNode<Item> myRoot;
	
  public BinaryTree ( ) {
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
	
  /** Dump THIS, with indentation showing structure. */
  public void print ( ) {
    if (myRoot != null) {
      print (myRoot, 0);
    }
  }

  /** Dump ROOT indented by INDENT indentation units. */
  void print (TreeNode<?> root, int indent) {
	  if (root.right != null) print (root.right, indent+1);
    println (root.myItem, indent);
    if (root.left != null) print (root.left, indent+1);
  }

  /** Number of spaces in one indentation unit. */ 
  static int INDENTATION = 4;

  /** Print OBJ, indented by INDENT indentation units, followed by a 
   *  newline. */
  static private void println (Object obj, int indent) {
    for (int k = 0; k < indent * INDENTATION; k += 1) 
      System.out.print (" ");
    System.out.println (obj);
  }

  private static <Item> void print (BinaryTree<Item> t, String description) {
    System.out.println (description + " in preorder");
    t.printPreorder ( );
    System.out.println (description + " in inorder");
    t.printInorder ( );
    System.out.println ( );
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

	  System.out.println("SAMPLE TREE 3 PRINTOUT: ------------------");  
	  sampleTree3 ( ).print();
	  System.out.println("BALANCED TREE PRINTOUT: ------------------"); 
	    balancedTree ( ).print();
	    System.out.println("EXPRESSION TREE PRINTOUT: ------------------");
	    BinaryTree<String> expressionTreeExample = exprTree("((a+(5*(a+b)))+(6*5))");
	    expressionTreeExample.print();
	    System.out.println("OPTIMIZED EXPRESSION TREE PRINTOUT: ------------------");
	    optimize(expressionTreeExample);
	    expressionTreeExample.print();
	    
	    System.out.println("OPTIMIZED EXPRESSION TREE WITH ONLY NUMBERS PRINTOUT: ------------------");
	    BinaryTree<String> expressionTreeNumbers = exprTree("((5+(5*(2+3)))+(6*5))");
	    optimize(expressionTreeNumbers);
	    expressionTreeNumbers.print();
	    
	    System.out.println("OPTIMIZED EXPRESSION TREE WITH SOME NUMBERS PRINTOUT: ------------------");
	    BinaryTree<String> expressionTreePart = exprTree("((a+(5*(9+1)))+(6*5))");
	    optimize(expressionTreePart);
	    expressionTreePart.print();
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

  /** The expression tree corresponding to S.  S is a legal, fully
   *  parenthesized expressions, contains no blanks, and involves 
   *  only the operations + and *, and leaf labels (which can be
   *  any string of characters other than *, + and parentheses). */
  public static BinaryTree<String> exprTree (String s) {
    BinaryTree<String> result = new BinaryTree<String> ( );
    result.myRoot = result.exprTreeHelper (s);
    return result;
  }

  private TreeNode<String> exprTreeHelper (String expr) {
    if (expr.charAt (0) != '(') {
      return new TreeNode(expr, null, null);
    } else {
      // expr is a parenthesized expression.
      // Strip off the beginning and ending parentheses,
      // find the main operator (an occurrence of + or * not nested
      // in parentheses, and construct the two subtrees.
      int nesting = 0;
      int opPos = 0;
      for (int k = 1; k < expr.length()-1; k += 1) {
    	  if (expr.charAt(k) == '(') {
    		  while (expr.charAt(k) != ')') k += 1;
    	  }
		if (expr.charAt(k) == '*' || expr.charAt(k) == '+') {
			opPos = k;
			break;
		}
      }
      String opnd1 = expr.substring (1, opPos);
      String opnd2 = expr.substring (opPos+1, expr.length()-1);
      String op = expr.substring (opPos, opPos+1);
      return new TreeNode (op, exprTreeHelper(opnd1), exprTreeHelper(opnd2));
    }
  }

  /** Destructively modify EXPR into a mathematically equivalent expression
   *  in which all subexpressions containing only numerals are replaced by
   *  equivalent numerals. */
  public static void optimize (BinaryTree<String> expr) {
    expr.myRoot = optimizeMe(expr.myRoot);
  }
  
  private static TreeNode<String> optimizeMe (TreeNode<String> node) {
	  if (node.left == null && node.right == null) {
		  return node;
	  }
	  if (node.left.myItem.charAt(0) == '+' || node.left.myItem.charAt(0) == '*') {
		  node.left = optimizeMe (node.left);
	  }
	  if (node.right.myItem.charAt(0) == '+' || node.right.myItem.charAt(0) == '*') {
		  node.right = optimizeMe (node.right);
	  }
	  try {
		  node.myItem = evalArithmetic (node.myItem, Integer.parseInt(node.left.myItem), Integer.parseInt(node.right.myItem)) + "";
		  node.left = node.right = null;
	  } catch (NumberFormatException e) { }
	  return node;
  }
  
  private static int evalArithmetic (String operator, int arg1, int arg2) {
	  if (operator.equals("+")) {
		  return arg1 + arg2;
	  } else if (operator.equals("*")) {
		  return arg1 * arg2;
	  } else {
		  throw new UnsupportedOperationException();
	  }
  }


}

