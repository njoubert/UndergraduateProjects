class Shuffled {
    public static void main (String [] args) {
      int x = 3;
      if (x > 2) {
	System.out.print ("a");
      }
      while (x > 0) {
	x = x - 1;
	System.out.print ("-");
	if (x == 2) {
	  System.out.print ("b c");
	}
	if (x == 1) {
	  System.out.print("d");
	  x = x - 1;
	}
      }
    }
}
