/* HW #8, Problem 2. */

import java.util.Collections;
import java.util.*;

class Ranges {
	/** Assuming that RANGES contains two-element arrays of integers,
	 *  <x,y> with x <= y, representing intervals of ints, this returns 
	 *  a List of the intervals of [0 .. MAX] that are NOT covered by one of 
	 *  the intervals in RANGES. The resulting list consists of non-overlapping 
	 *  intervals ordered in increasing order of their starting points. */
	static List<int[]> uncovered (List<int[]> ranges, int max) {
		Comparator blah = new MyComparator();
		Collections.sort(ranges, blah);
		int startInt = 0;
		int[] newR, currR;
		List<int[]> out = new ArrayList<int[]>();
		while (!ranges.isEmpty()) {
			newR = new int[2];
			newR[0] = startInt;
			currR = ranges.remove(0);
			newR[1] = currR[0] - 1;
			startInt = currR[1] + 1;
			out.add(newR);
		}
		newR = new int[2];
		newR[0] = startInt;
		newR[1] = max;
		out.add(newR);
		return out;
	}
	

	
	public static void main (String[] args) {
		int[] a = {5, 20};
		int[] b = {50, 100};
		int[] c = {120, 200};
		ArrayList<int[]> blah = new ArrayList<int[]>();
		blah.add(a); blah.add(b); blah.add(c);
		
		for (int[] o : uncovered(blah, 5000)) {
			System.out.println(o[0] + " - " + o[1]);
		}
	}
	
}

class MyComparator implements Comparator<int[]> {
	public int compare(int[] o1, int[] o2) {
		if (o1[1] < o2[0]) return -1;
		if (o1[0] < o2[0]) return 1;
		return 1; 
	}
}
