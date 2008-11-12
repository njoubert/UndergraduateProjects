package util;

import java.util.*;

public class QuadTree implements Set2D {
	
	/** An empty set whose bounding rectangle has lower-left coordinates
	 *  (LLX,LLY) and upper-right coordinates (URX,URY).  */
	public QuadTree (double llx, double lly, double urx, double ury) {
		this.llx = llx; this.lly = lly; this.urx = urx; this.ury = ury;
		root = new RootNode(this);
		points = new Hashtable(128);
	}
	
	/** A QuadTree whose contents are copied from SET and whose bounding
	 *  rectangle has lower-left coordinates (LLX,LLY) and upper-right 
	 *  coordinates (URX,URY).   Requires that all objects in SET reside
	 *  within the bounding rectangle. */
	public QuadTree (Set2D inSet, double llx, double lly, double urx, double ury) {
		this(llx, lly, urx, ury);
		int id;
		Set2DIterator iterator = inSet.iterator(llx, lly, urx, ury);
		while (iterator.hasNext()) {
			id = iterator.next();
			this.add(id, inSet.x(id), inSet.y(id));
		}
	}
	
	/* PUBLIC METHODS.  See Set2D.java for documentation */
	
	public int size () {
		return points.size();
	}
	
	/**
	 * Inserts the given id in the tree. Stores the x-y values in a hashtable, and
	 * uses the insert method of the nodes to insert it into the appropriate place
	 * in the tree. This is awesome because by simply caaling the insert of the root,
	 * it automatically trickles down into its right spot because of the
	 * smart nodes we use.
	 */
	public void add (int id, double x0, double y0) {
		if (x0 >= urx() || x0 < llx() || y0 < lly() || y0 >= ury() || id < 0 || points.containsKey(id)) throw new IllegalArgumentException();
		double[] point = {x0, y0};
		points.put(id, point);
		root = root.insert(id);
	}
	
	public void set (int id, double x, double y) {
		if (! exists (id))
			throw new IllegalArgumentException ("nonexistent id given to set");
		remove (id);
		add (id, x, y);
	}
	
	public void remove (int id) {
		if (points.containsKey(id)) {
			root = root.remove(id);
			points.remove(id);
		}
	}
	
	public boolean exists (int id) {
		return points.containsKey(id);
	}
	
	public double llx () { return llx; }
	
	public double lly () { return lly; }
	
	public double urx ()  { return urx; }
	
	public double ury ()  { return ury; }
	
	public double x (int id) {
		if (points.containsKey(id)) {
			return points.get(id)[0];
		} else return Double.NaN;
	}
	
	public double y (int id) {
		if (points.containsKey(id)) {
			return points.get(id)[1];
		} else return Double.NaN;
	}
	
	public Set2DIterator iterator () {
		return new QuadTreeIterator();
	}
	
	public Set2DIterator iterator (double xl, double yl, double xu, double yu) {
		return new QuadTreeIterator(xl, yl, xu, yu);
	}
	
	/** The root node, which contains (along with its subtrees) all points
	 *  in THIS tree. */
	private QuadTreeNode root;
	
	private Hashtable<Integer, double[]> points;
	
	private double llx, lly, urx, ury;
	
	/*
	 * ITERATER CLASSES:
	 */
	
	/**
	 * Offers two views of the set, in order of ID.
	 * One shows the complete set, the other shows only
	 * the elements that lie within a certain bounding box.
	 * i.e. elements (x,y) where:
	 * 	ury > y >= lly
	 *  urx > x >= llx
	 */
	private class QuadTreeIterator implements Set2DIterator {
		TreeSet<Integer> elements;
		
		/* For the unbounded iterator */
		public QuadTreeIterator() {
			elements = new TreeSet();
			analyze(((RootNode)root).children[0]);
			analyze(((RootNode)root).children[1]);
			analyze(((RootNode)root).children[2]);
			analyze(((RootNode)root).children[3]);
		}
		
		private void analyze(QuadTreeNode node) {
			if (node.type().equals("leaf")) {
				elements.add(((LeafNode)node).id);
			} else if (node.type().equals("inner")) {
				analyze(((InnerNode)node).children[0]);
				analyze(((InnerNode)node).children[1]);
				analyze(((InnerNode)node).children[2]);
				analyze(((InnerNode)node).children[3]);
			}
		}
		
		/* For the bounded iterator */
		
		double xl, yl, xu, yu;
		
		public QuadTreeIterator(double llx, double lly, double urx, double ury) {
			elements = new TreeSet();
			xl = llx; yl = lly; xu = urx; yu = ury;
			RootNode nodeHandle = (RootNode)root;
			
			if (!nodeHandle.children[0].type().equals("empty") && 
					xu >= nodeHandle.children[0].llx() && 
					yu >= nodeHandle.children[0].lly()) analyzeBox(nodeHandle.children[0]);
			
			if (!nodeHandle.children[1].type().equals("empty") && 
					xl < nodeHandle.children[1].urx() && 
					yu > nodeHandle.children[1].lly()) analyzeBox(nodeHandle.children[1]);
			
			if (!nodeHandle.children[2].type().equals("empty") && 
					xl < nodeHandle.children[2].urx() && 
					yl < nodeHandle.children[2].ury()) analyzeBox(nodeHandle.children[2]);
			
			if (!nodeHandle.children[3].type().equals("empty") && 
					xu >= nodeHandle.children[3].llx() && 
					yl <= nodeHandle.children[3].ury()) analyzeBox(nodeHandle.children[3]);
		}
		
		/**
		 * The recursive analyze method for bounded view of the 
		 * Iterator. Uses a decision structure to add only quadrants that 
		 * fall within the bounding rectangle of the view.
		 * Objects always fall towards the upper right packadge if they lie
		 * on a bounding rectangle. We want to include these objects
		 * if we call an iterator that has a bound on 
		 * DECISION STRUCTURE:
		 * Quadrants that satisfy these boundaries are included in the iterator.
		 * 	for children[0]:
		 * 		xu >= llx() 
		 * 		yu >= lly()
		 * 	for children[1]:
		 * 		xl < urx()
		 * 		yu > lly()	
		 * 	for children[2]:
		 * 		xl < urx()
		 * 		yl < ury()
		 * 	for children[3]:
		 * 		xu >= llx()
		 * 		yl <= ury()
		 * @param node
		 */
		private void analyzeBox(QuadTreeNode node) {
			if (node.type().equals("leaf")) {
				if (node.isWithin(((LeafNode)node).id, xl, yl, xu, yu)) elements.add(((LeafNode)node).id);
			} else if (node.type().equals("inner")) {
				InnerNode nodeHandle = (InnerNode) node;
				//First Quad
				if (!nodeHandle.children[0].type().equals("empty") && 
						xu >= nodeHandle.children[0].llx() && 
						yu >= nodeHandle.children[0].lly()) analyzeBox(nodeHandle.children[0]);
				//Second Quad
				if (!nodeHandle.children[1].type().equals("empty") && 
						xl < nodeHandle.children[1].urx() && 
						yu > nodeHandle.children[1].lly()) analyzeBox(nodeHandle.children[1]);
				//Third Quad
				if (!nodeHandle.children[2].type().equals("empty") && 
						xl < nodeHandle.children[2].urx() && 
						yl < nodeHandle.children[2].ury()) analyzeBox(nodeHandle.children[2]);
				//Fourth Quad
				if (!nodeHandle.children[3].type().equals("empty") && 
						xu >= nodeHandle.children[3].llx() && 
						yl <= nodeHandle.children[3].ury()) analyzeBox(nodeHandle.children[3]);
			}
		}
		
		/* same across both iterators */
		public boolean hasNext() {
			return !elements.isEmpty();
		}
		
		public int next() {
			Integer id = elements.first();
			elements.remove(id);
			return id;
		}
	}
	
}

