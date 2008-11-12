/** java HW1Test should test the methods defined in Progs. */
public class HW1Test {

	public static void printIntList (IntList L) {
		while (L != null) {
			System.out.print(L.head + " ");
			L = L.tail;
		}
	}
	
  public static void main (String[] args) {
    report ("factorSum", test_factorSum ());
    report ("printSociablePairs", test_printSociablePairs ());
    report ("dcatenate", test_dcatenate ());
    report ("sublist", test_sublist ());
    report ("dsublist", test_dsublist ());
  }

  private static void report (String name, boolean isOK) {
    if (isOK) 
      System.out.printf ("%s OK.%n", name);
    else
      System.out.printf ("%s FAILS.%n", name);
  }

  // Replace the bodies of the functions below with something serious.

  private static boolean test_factorSum () {
	  boolean returnVal = true;
	  if (Progs.factorSum(284) != 220) returnVal = false;
	  if (Progs.factorSum(220) != 284) returnVal = false;
    return returnVal;
  }

  private static boolean test_printSociablePairs () {
	  //	 someting
    return true;
  }

  private static boolean test_dcatenate () {
		 IntList blah = new IntList(1, null);
		 IntList nobby = new IntList(3, null);
		 IntList nah = new IntList (5, null);
		 blah.tail = nah;
		 Progs.dcatenate(blah, nobby);
		 int test = blah.head + blah.tail.head + blah.tail.tail.head;
		 if (test == 9)
			 return true;
    return false;
  }

  private static boolean test_sublist () {
	  IntList ell = new IntList(2, new IntList(3, new IntList(5, new IntList(9, new IntList(16, new IntList(20, null))))));
	  IntList newell = Progs.sublist(ell, 2, 2);
	  if (newell.head != 5) { 
		  return false;
	  } else if (newell.tail.head != 9) {
		  return false;
	  } else if (newell.tail.tail != null) {
		  return false;
	  } else {
		  return true;
	  }
  }

  private static boolean test_dsublist () {
	  IntList ell = new IntList(2, new IntList(3, new IntList(5, new IntList(9, new IntList(16, new IntList(20, null))))));
	  IntList newell = Progs.dsublist(ell, 2, 2);
	  if (newell.head != 5) { 
		  return false;
	  } else if (newell.tail.head != 9) {
		  return false;
	  } else if (newell.tail.tail != null) {
		  return false;
	  } else {
		  return true;
	  }
  }

  
}
