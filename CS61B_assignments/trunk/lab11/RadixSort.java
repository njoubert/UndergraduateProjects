import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;
import java.util.List;
import java.util.ListIterator;
import java.util.ArrayList;

class RadixSort {
	
	/** The command 'java RadixSort FILE' reads FILE and sorts its lines. */
	public static void main (String[] args) {
		ArrayList<String> lines = new ArrayList<String> ();
		try {
			if (args.length == 0) {
				System.err.println ("Usage: java RadixSort FILE");
				System.exit (1);
			}
			Scanner inp = new Scanner (new File (args[0]));
			while (inp.hasNextLine ())
				lines.add (inp.nextLine ().trim ());
			long start = System.currentTimeMillis ();
			radixSort (lines);
			long end = System.currentTimeMillis ();
			checkSort (lines);
			for (String line : lines)
				System.out.println (line);
			System.err.printf ("%nSorted %d lines in %.2f seconds.%n",
					lines.size (), (end-start) * 0.001);
		} catch (FileNotFoundException e) {
			System.err.printf ("Could not file file '%s'.%n", args[0]);
			System.exit (1);
		}
	}
	
	/** Checks that LINES is in non-descending order. */
	static private void checkSort (List<String> lines) {
		for (int i = 1; i < lines.size (); i += 1)
			if (lines.get (i-1).compareTo (lines.get (i)) > 0)
				throw new AssertionError ("Array out of order at position " + i );
	}
	
	/** Sort LINES into non-descending order.  Assumes that all characters
	 *  in LINES are non-null 7-bit ASCII (that is, have codes in the 
	 *  range 1..127). */
	static void radixSort (List<String> lines) {
		String[] lines1 = lines.toArray (new String[lines.size ()]);
		
		int L;
		L = 0;
		for (String S : lines1)
			L = Math.max (L, S.length ());
		
		for (int i = L-1; i >= 0; i -= 1) {
			sortByOneChar (lines1, i);
		}
		
		int j;
		j = 0;
		for (ListIterator<String> iter = lines.listIterator (); 
		iter.hasNext (); ) {
			iter.next ();
			iter.set (lines1[j]);
			j += 1;
		}
	}
	
	/** Sort A in non-descending order stably by character #K. Strings
	 *  whose length is <= K sort before all other strings. */
	private static void sortByOneChar (String[] A, int k) {
		// Use a distribution sort (section 8.5, figure 8.6 of 
		// Data Structures) to perform the sort on character #K. You
		// won't need to find the maximum and minimum characters, 
		// because radixSort assumes all characters are in the range 
		// 1 .. 127 (plus you can use 0 for the non-existent character).
		

	}
	
}

