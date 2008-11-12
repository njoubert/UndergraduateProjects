package tracker;

import java.util.Collections;
import java.util.Scanner;
import java.util.ArrayList;
import java.util.Comparator;
import util.Set2D;

/**
 * Handles the command "within"
 */
class CommandWithin extends Command {
	
	String parse (String commandText, Scanner inp, State theState) throws ProblemException, ParsedException {
		if (!commandText.equals("within")) return null;
		
		double[] inputPoint = extractArray (inp, 1);
		
		return execute(theState, inputPoint[0]);
	}
	
	private String execute(State theState, double d) throws ProblemException {
		ArrayList<int[]> pairs = theState.within(d);
		Collections.sort(pairs, new CompareIDs());
		for (int[] pair : pairs) {
			System.out.printf("%s:(%.4g,%.4g) %s:(%.4g,%.4g)%n", pair[0], theState.getX(pair[0]), theState.getY(pair[0]), pair[1], theState.getX(pair[1]), theState.getY(pair[1]));
		}
		return "within";
	}
	
	class CompareIDs implements Comparator<int[]> {
		public int compare(int[] pairA, int[] pairB) {
			
			if (pairA[0] < pairB[0]) return -1;
			if (pairA[0] == pairB[0]) {
				if (pairA[1] < pairB[1]) return -1;
				if (pairA[1] == pairB[1]) return 0;
			}
			return 1;
		}
	}
	
	
	
	/*private String execute(State theState, double d) throws ProblemException {
		util.Set2DIterator aroundCurrentPoint, allPoints;
		double x1, y1, x2, y2;
		int id1, id2;
		allPoints = theState.iterator();
		while (allPoints.hasNext()) {
			id1 = allPoints.next();
			x1 = theState.getX(id1);
			y1 = theState.getY(id1);
			aroundCurrentPoint = theState.iterator(x1 - d, y1 - d, x1 + d, y1 + d);
			while (aroundCurrentPoint.hasNext()) {
				id2 = aroundCurrentPoint.next();
				x2 = theState.getX(id2);
				y2 = theState.getY(id2);
				if (id1 < id2 && theState.distance(x1, y1, x2, y2) <= d) {
					System.out.printf("%s:(%.4g, %.4g) %s:(%.4g, %.4g)%n", id1, x1, y1, id2, x2, y2);
				}
			}
		}
		
		return "within";
	}*/
	
	
	 
	
}