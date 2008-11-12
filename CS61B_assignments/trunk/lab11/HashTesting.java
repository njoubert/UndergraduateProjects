import java.util.*;
import java.io.*;

public class HashTesting {

  /** An array of N*N four-character strings of the form xxyy, for 
   *  all pairs 1 <= x, y <= N. */
  static ArrayList<String> set0 (int N) {
    ArrayList<String> result = new ArrayList<String> (N * N);
    for (int i = 1, n = 0; i <= N; i += 1)
      for (int j = 1; j <= N; j += 1, n += 1) {
	result.add ("" + (char) i + (char) i + (char) j + (char) j);
      }
    return result;
  }

  /** An array of N*N four-character strings of the form xXyY, for 
   *  X = 2**16 - x*31 - 1 and Y == 2**16 - y*31 - 1, for all pairs
   *  of values 1 <= x, y <= N.  (Here, ** denotes exponentiation). */
  static ArrayList<String> set1 (int N) {
    ArrayList<String> result = new ArrayList<String> (N * N);

    for (int i = 1, n = 0; i <= N; i += 1)
      for (int j = 1; j <= N; j += 1, n += 1) {
	result.add ("" + (char) i + (char) (0xffff - 31*i) 
		    + (char) j + (char) (0xffff - 31*j));
      }
    return result;
  }

  /** Add all objects in ALLOBJECTS to an initially empty HashSet, check
   *  that all the objects in CHECKOBJECTS are in that set, and 
   *  print the time required to do so. */
  static <T> void timeOne (Set<T> items,
			   List<? extends T> allObjects, 
			   List<? extends T> checkObjects) {
    long start = System.currentTimeMillis ();
    for (T s : allObjects)
      items.add (s);
    for (Object s : checkObjects)
      if (! items.contains (s))
	System.err.printf ("Did not find '%s'%n", s);
    long end = System.currentTimeMillis ();
    double totalOps = allObjects.size () + checkObjects.size ();

    System.out.printf ("Added %d items and checked %d items in %.1f seconds (%.2g msec/string)%n",
		       allObjects.size (), checkObjects.size (),
		       (end-start) * 0.001,
		       (end-start) / totalOps);
  }

  
  /** Read strings from FILE, store them in a HashSet that uses a hashing
   *  function that looks only at every SKIPth character, timing how long
   *  it takes to store and retrieve them. */
  static void test1 (String file, String skip) {
    int k = Integer.parseInt (skip);
    ArrayList<String1> items = new ArrayList<String1> ();
    try {
      Scanner inp = new Scanner (new File (file));
      while (inp.hasNext ())
	items.add (new String1 (inp.next (), k));
      System.out.printf ("Strings from %s, skip=%d:%n\t", file, k);
      timeOne (new HashSet<Object> (), items, items);
    } catch (FileNotFoundException e) {
      System.err.printf ("Could not open %s%n", file);
    }
  }

  /** Create N*N strings of the form "xxyy", where x and y vary 
   *  independently between 1 and N.  Time how long it takes to add
   *  them to a HashSet and check that they are there. */
  static void test2 (String N) {
    List<String> set = set0 (Integer.parseInt (N));
    System.out.print ("Strings xxyy: ");
    timeOne (new HashSet<Object> (), set, set);
  }

  /** Create N*N strings of the form "xXyY", where x and y vary 
   *  independently between 1 and N, X = 2**16 - 31x - 1 and
   *  Y = 2**16 - 31y - 1. Time how long it takes to add
   *  them to a HashSet and check that they are there. */
  static void test3 (String N) {
    List<String> set = set1 (Integer.parseInt (N));
    System.out.print ("Strings xXyY: ");
    timeOne (new HashSet<Object> (), set, set);
  }

  /** Add the strings in ARGS[1..] to a TreeSet and HashSet as FoldedStrings,
   *  then check that their upper-case versions are in the sets also. */
  static void test4 (String[] args) {
    System.out.println ("Folded strings....");
    TreeSet<FoldedString> set0 = new TreeSet<FoldedString> ();
    HashSet<FoldedString> set1 = 
      new HashSet<FoldedString> (args.length-1, 2.0f);
    for (String x : args) {
      if (x == args[0])
	continue;
      FoldedString s = new FoldedString (x);
      set0.add (s);
      set1.add (s);
    }
    for (String x : args) {
      if (x == args[0])
	continue;
      FoldedString s = new FoldedString (x.toUpperCase ());
      if (! set0.contains (s)) 
	System.out.printf ("    TreeSet does not contain '%s'%n", s);
      if (! set1.contains (s)) 
	System.out.printf ("    HashSet does not contain '%s'%n", s);
    }
  }      

  public static void main (String[] args) { 
	  if (args.length == 0) {
		  ArrayList<String> strs = set1(16);
		  for (String str : strs) {
			  System.out.println(str + " ");
		  }
	  }
	  else if (args[0].equals ("test1"))
		  test1 (args[1], args[2]);
	  else if (args[0].equals ("test2"))
		  test2 (args[1]);
	  else if (args[0].equals ("test3"))
		  test3 (args[1]);
	  else if (args[0].equals ("test4"))
		  test4 (args);
	  else 
		  System.err.println ("I don't know that test.");
	  
  }
  
}


/** A wrapper class for Strings.
 *  Implements only public operations of Object and a constructor that takes
 *  a String.  The toString method returns the constructor's argument. */
class String1 {

  /** A new String1 that represents the string S and whose hash function
   *  looks at every Kth character.   When K==1, the hash function is
   *  identical to that for java.lang.String. */
  public String1 (String s, int k) {
    rep = s;
    skip = k;
  }

  /** This is a duplicate of Java's actual hash function on String. */
  public int hashCode () {
    int h;
    h = 0;
    for (int i = 0, len = rep.length (), step = skip; i < len; i += step)
      h = h*31 + rep.charAt (i);
      return h;
  }

  public String toString () {
    return rep;
  }

  public boolean equals (Object obj) {
    return (obj instanceof String1)
      && rep.equals (((String1) obj).rep);
  }

  private String rep;
  private int skip;
}

/** A wrapper class for String whose .equals and .compareTo methods ignore
 *  the difference between upper and lower case letters. */
class FoldedString implements Comparable<FoldedString> {
  
  /** A new FoldedString that represents the string S. */
  public FoldedString (String s) {
    rep = s;
  }

  public String toString () {
    return rep;
  }

  public int hashCode () {
	  	String temp = rep.toLowerCase();
	    int h;
	    h = 0;
	    for (int i = 0; i < rep.length(); i += 1)
	      h = h*31 + temp.charAt(i);
	   return h;
   
    //return rep.hashCode ();
  }

  public boolean equals (Object obj) {
    return (obj instanceof FoldedString) 
      && rep.equalsIgnoreCase (((FoldedString) obj).rep);
  }

  public int compareTo (FoldedString anotherString) {
    return rep.compareToIgnoreCase (anotherString.rep);
  }
  
  private String rep;

}

