package tracker;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.TreeSet;
import java.util.Iterator;
import java.lang.Double;
import util.*;



/**
 * The State of the current particle system.
 * Stores all points as arrays of type Double in the form
 * p = {x, y, vx, vy}
 * 
 * Functions like a set - does not allow duplicate IDs
 * (does not process the position of the points).
 * 
 * The implementation of this is designed to allow for fast
 * lookup, adding or changing of position data according to ID.
 * Thus the methods that work with ID all run in constant time.
 * 
 * @author Niels Joubert cs61b-bo
 *
 */

public class State {
	
	/**
	 * Sets all the beginning values so that nothing can 
	 * be added to this Set until some initial set-up is done:
	 * the boundaries of the set needs to be set, and a
	 * radius needs to be associated with particles.
	 * @param myMain
	 */
	public State(Main myMain) {
		this.myMain = myMain;
		velocities = new Hashtable(128);
		myTree = new QuadTree(0, 0, 0, 0);
		rad = Double.POSITIVE_INFINITY;
		D = Double.POSITIVE_INFINITY;
	}
	
	public State(Main myMain, double xlow, double ylow, double xhigh, double yhigh, double rad) {
		this(myMain);
		moveBoundaries(xlow, ylow, xhigh, yhigh);
		setRadius(rad);
	}
	
	public State(Main myMain, double xlow, double ylow, double xhigh, double yhigh, double rad, TrackerFrame gui) {
		this(myMain);
		moveBoundaries(xlow, ylow, xhigh, yhigh);
		setRadius(rad);
		this.gui = gui;
		changeGUI(gui);
	}
	
	/**
	 * @return A pointer to the Main class, that runs the whole show.
	 */
	public Main getMyMain() { return this.myMain; }
	
	
				/*
				 * Data Structure Methods
				 */
	
	
	/**
	 * Adds a new element to the state of the program.
	 * Returns false if an element with the same ID already exists.
	 * @param ID
	 * @param x, y, vx, vy - position data as doubles.
	 * @return true if success, false if ID already exists
	 */
	public boolean add(int ID, double x, double y, double vx, double vy) throws ProblemException {
		try {
			double[] v = {vx, vy};
			//Added to move particles if they are outside box.
			if ((x - rad) < llx()) {
				x = llx() + rad;
			} else if ((x + rad) > urx()) {
				x = urx() - rad;
			}
			if ((y - rad) < lly()) {
				y = lly() + rad;
			} else if ((y + rad) > ury()) {
				y = ury() - rad;
			}
			//if ((x - rad) < llx() || (x + rad) > urx() || (y - rad) < lly() || (y + rad) > ury())
				//throw new ProblemException("You cannot add a particle that has its circumfence outside the bounding box.");
			
			for (Integer points : nearCircle(iterator(x - 2.0*rad, y - 2.0*rad, x + 2.0*rad, y + 2.0*rad), x, y, 2.0*rad)) {
				if (distance(getX(points), getY(points), x, y) < 2.0 * rad * (1.0 - 1.0/(1L<<40))) throw new ProblemException("You cannot add a particle that is within 2*radius of another particle");
			}
			myTree.add(ID, x, y);
			velocities.put(new Integer(ID), v);
			return true;
		} catch (IllegalArgumentException e) {
			throw new ProblemException("The ID already exists!");
		}
	}
	
	public boolean add(int ID, double[] point) throws ProblemException {
		return add(ID, point[0], point[1], point[2], point[3]);
	}
	
	/**
	 * Changes the value of the given ID. Returns false if the ID is not
	 * already in the State.
	 * @param ID
	 * @param x, y, vx, vy - position data as doubles.
	 * @return true of successful, false if element does not exist.
	 */
	public boolean set (int ID, double x, double y, double vx, double vy) {
		try {
			double[] v = {vx, vy};
			myTree.set(ID, x, y);
			velocities.put(new Integer(ID), v);
			return true;
		} catch (IllegalArgumentException e) {
			return false;
		}
	}
	
	/**
	 * Checks whether the current ID is contained in this State.
	 * @param ID - integer ID
	 * @return - true if ID is in State
	 */
	public boolean contains(int ID) {
		return myTree.exists(ID);
	}
	
	/*
	 * Procedures to retrieve the position and velocity data from the State of the current simulation.
	 */
	
	/**
	 * Gets the position data of ID.
	 * @return - a double array of the form {x, y, vx, vy}
	 */
	public double[] get(int ID) {
		
		double[] point = new double[4];
		System.arraycopy(velocities.get(new Integer(ID)), 0, point, 2, 2);
		point[0] = myTree.x(ID);
		point[1] = myTree.y(ID);
		return point;
		
	}
	public double getX(int ID) {
		return myTree.x(ID);
	}
	public double getY(int ID) {
		return myTree.y(ID);
	}
	public double getVX(int ID) {
		return velocities.get(new Integer(ID))[0];
	}
	public double getVY(int ID) {
		return velocities.get(new Integer(ID))[1];
	}
	
	/*
	 * Procedures to retrieve the parameters of the bounding box
	 */
	
	public double llx() { return myTree.llx(); }
	public double lly() { return myTree.lly(); }
	public double urx() { return myTree.urx(); }
	public double ury() { return myTree.ury(); }
	
	/*
	 * Procedures to get an iterator 
	 */
	public Set2DIterator iterator () { return myTree.iterator(); }
	public Set2DIterator iterator (double xl, double yl, double xu, double yu) { return myTree.iterator(xl, yl, xu, yu); }
	
	
					/*
					 * Range Queries
					 */
	
	public TreeSet<Integer> nearCircle(Set2DIterator iterator, double x, double y, double d) {
		TreeSet<Integer> points = new TreeSet();
		double currentX, currentY;
		int id;
		while (iterator.hasNext()) {
			id = iterator.next();
			currentX = getX(id);
			currentY = getY(id);
			if (distance(x, y, currentX, currentY) <= d) {
				points.add(new Integer(id));
			}
		}
		return points;
	}
	
	public TreeSet<Integer> nearBox(Set2DIterator iterator) {
		TreeSet<Integer> points = new TreeSet();
		while (iterator.hasNext()) {
			points.add(new Integer(iterator.next()));
		}
		return points;
	}
	
	/**
	 * Uses the same algorithm as the within Command to find all objects
	 * within a certain radius of each other.
	 * @param d - radius
	 * @return an ArrayList or 2-element arrays of the form {id1, id2}
	 * endures that the id1's are in ascending order, and no
	 * duplicate pairs exist.
	 */
	public ArrayList<int[]> within(double d) {
		ArrayList<int[]> pointsWithin = new ArrayList();
		Set2DIterator aroundCurrentPoint, allPoints;
		double[] p1, p2;
		int id1, id2;
		allPoints = iterator();
		while (allPoints.hasNext()) {
			id1 = allPoints.next();
			p1 = this.get(id1);
			aroundCurrentPoint = iterator(p1[0] - d, p1[1] - d, p1[0] + d, p1[1] + d);
			while (aroundCurrentPoint.hasNext()) {
				id2 = aroundCurrentPoint.next();
				p2 = this.get(id2);
				if (id1 < id2 && distance(p1[0], p1[1], p2[0], p2[1]) <= d) {
					int[] points = {id1, id2};
					pointsWithin.add(points);
				}
			}
		}
		return pointsWithin;
	}
	
	
						/*
						 * Management Procedures
						 */
	
	/**
	 * moveBoundaries attempts to stretch the boundaries of the bounding box of the current 
	 * area of simulation. Boundaries can only be moved outward from the current position, 
	 * and a ProblemException is thrown if boundaries are moves inwards.
	 * @param llx, lly, urx, ury - the new coordinates of the bounding box
	 */
	public boolean moveBoundaries(double xlow, double ylow, double xhigh, double yhigh) {
		if (xlow > llx() || ylow > lly() || xhigh < urx() || yhigh < ury())
			return false;
		else  {
			myTree = new QuadTree(myTree, xlow, ylow, xhigh, yhigh);
			return true;
		}
	}
	
	/**
	 * Sets the radius for all the objects we are simulating.
	 * The radius cannot be decreased, and must be larger than zero.
	 * Returns true if the method succeeds
	 * @param r
	 */
	public boolean setRadius(double r) {
		if (r > 0 && r <= rad) {
			rad = r;
			D = r * 2.0;
			return true;
		} else return false;
	}
	
	/**
	 * @return the current value for the radius of particles.
	 */
	public double getRadius() {
		return rad;
	}
	
	
						/*
						 * MATH PROCEDURES:
						 */
	
	/**
	 * Calculates the straight-line distance between the (x1, y1) and (x2, y2) point
	 * using the Pythagorean Theorem.
	 * @return the distance between given points.
	 */
	public static double distanceSquare(double x1, double y1, double x2, double y2) {
		return (y2 - y1)*(y2 - y1) + (x2 - x1)*(x2 - x1);
	}
	public static double distance(double x1, double y1, double x2, double y2) {
		return Math.sqrt(distanceSquare(x1, y1, x2, y2));
	}
	public static double pyth(double x1, double y1) {
		return distance(0, 0, x1, y1);
	}
	

	/**
	 * Simulates the change in positions and velocities of a system over time.
	 * @param totalTime
	 * @return
	 */
	public void simulate(double totalTime) throws ProblemException  {
		double Vmax, deltaT, tc;
		State newState; int id; double point[], point2[];
		Set2DIterator allPoints;
		//if (D <= 0) POSSIBLY CHECK FOR RADIUS SETTING!
		updateGUI();
		if (totalTime > 0) {
			Vmax = this.findMaxVelocity();
			deltaT = Math.min(totalTime, D / Vmax);
			tc = Math.min(deltaT, findTimeToCollision());
			
			newState = new State(myMain, llx(), lly(), urx(), ury(), rad, gui);
			allPoints = iterator();
			while (allPoints.hasNext()) {
				id = allPoints.next();
				point = get(id);
				point[0] = point[0] + point[2]*tc;
				point[1] = point[1] + point[3]*tc;
				//wall collisions:
				if (point[0] - rad <= llx()) point[2] = -1.0 * point[2];
				if (point[0] + rad >= urx()) point[2] = -1.0 * point[2];
				if (point[1] + rad >= ury()) point[3] = -1.0 * point[3];
				if (point[1] - rad <= lly()) point[3] = -1.0 * point[3];
				newState.add(id, point);
			}
			
			/*
			 * Find all the points that is 2R from eachother. Calculate a new
			 * velocity vector for these.
			 */
			for (int[] colliders : newState.within(2.0*rad*(1.0 + 1.0/(1L<<20)))) {
				point = newState.get(colliders[0]);
				point2 = newState.get(colliders[1]);
				Physics.rebound(point, point2);
				if (!newState.set(colliders[0], point[0], point[1], point[2], point[3])) throw new ProblemException("In trying to update the velocity of a particle, the system failed in the set method");
				if (!newState.set(colliders[1], point2[0], point2[1], point2[2], point2[3])) throw new ProblemException("In trying to update the velocity of a particle, the system failed in the set method");
			}
			
			
			this.myMain.theState = newState;
			newState.simulate(totalTime - tc);
		}
		
	}
	
	public double findTimeToCollision() {
		double tCollision = Double.POSITIVE_INFINITY;
		double tempTC = 0;
		double[] point;
		int[] particles;
		Iterator<int[]> particlesInteraction = within(2.0*(D + rad)).iterator();
		Iterator<Integer> nearWallLeft = nearBox(this.iterator(llx(), lly(), llx() + rad + D, ury())).iterator();
		Iterator<Integer> nearWallRight = nearBox(this.iterator(urx() - D - rad, lly(), urx(), ury())).iterator();
		Iterator<Integer> nearWallTop = nearBox(this.iterator(llx(), ury() - rad - D, urx(), ury())).iterator();
		Iterator<Integer> nearWallBottom = nearBox(this.iterator(llx(), lly(), urx(), lly() + rad + D)).iterator();
		
		while (particlesInteraction.hasNext()) {
			particles = particlesInteraction.next();
			tempTC = Physics.collide(get(particles[0]).clone(), get(particles[1]).clone(), rad);
			if (tempTC < tCollision) tCollision = tempTC;
		}
		while (nearWallLeft.hasNext()) {
			point = get(nearWallLeft.next());
			if (point[2] < 0) {
				tempTC = Math.abs(((point[0] - rad) - llx()) / point[2]);
				if (tempTC < tCollision) tCollision = tempTC;
			}
		}
		
		while (nearWallRight.hasNext()) {
			point = get(nearWallRight.next());
			if (point[2] > 0) {
				tempTC = Math.abs((urx() - (point[0] + rad)) / point[2]);
				if (tempTC < tCollision) tCollision = tempTC;
			}
		}
		
		while (nearWallTop.hasNext()) {
			point = get(nearWallTop.next());
			if (point[3] > 0) {
				tempTC = Math.abs((ury() - (point[1] + rad)) / point[3]);
				if (tempTC < tCollision) tCollision = tempTC;
			}
		}
		
		while (nearWallBottom.hasNext()) {
			point = get(nearWallBottom.next());
			if (point[3] < 0) {
				tempTC = Math.abs(((point[1] - rad) - llx()) / point[3]);
				if (tempTC < tCollision) tCollision = tempTC;
			}
		}
		
		return tCollision;
	}
	
	/**
	 * Runs through the given set, comparing each element's velocity vector with
	 * the current maximum velocity to find the biggest velocity vector in out System
	 * @return the maximum velocity vector in this State.
	 */
	public double findMaxVelocity() {
		double Vmax = 0, v = 0;
		double[] currentVs;
		Iterator<double[]> vIter = velocities.values().iterator();
		while (vIter.hasNext()) {
			currentVs = vIter.next();
			v = pyth(currentVs[0], currentVs[1]);
			if (v > Vmax) Vmax = v;
		}
		return Vmax;
	}
	
	public void setGui(TrackerFrame gui) {
		this.gui = gui;
	}
	
	public TrackerFrame getGui() {
		return this.gui;
	}
	
	public void updateGUI() {
		if (gui != null) gui.repaint();
	}
	
	public void changeGUI(TrackerFrame gui) {
		gui.setState(this);
	}
	
	/**
	  *
	  * The private fields.
	  * The State is represented by two data structures:
	  * The Quadtree - for fast lookup according to position, stores IDs according to (x,y)
	  * The HashTable - for fast lookup according to ID, stores (vx, vy) according to ID
	  * 
	  * rad - Stores the radius of all particles
	  * myMain - Stores a pointer to the Main class that instantiated yourself
	  * 		Used by the simulation method when a new State is created non-destructively
	  * 		and needs to replace the current State.
	  * D - stores the maximum distance over which we search for Collisions during simulate.
	  */
	private Hashtable<Integer, double[]> velocities;
	private Set2D myTree;
	private double rad;
	private Main myMain;
	private double D;
	private TrackerFrame gui;
	
}