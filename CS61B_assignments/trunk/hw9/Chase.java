/**
 * Sample input:
 * 10 2 3 2 2 4 2 3 5 4 3 6 1 4 1 3 5 6 6 5 8 2 5 9 1 6 7 2 6 9 3 7 0 1 7 9 8 1 8 1 8 9 7
 * Should give that the borogrove gets eaten (in room 6 btw, although this is not necessary info)
 */


import java.util.*;

class Chase {
	/** Read in description of maze from the standard input and print out 
	 *  result of chase, as indicated in HW10, problem 1. */
	
	public static void main (String[] args) {
		int[][] graph;
		int v, w, d;
		double snarkV = 0.5;
		double borogroveV = 1.0;
		
		//Read in graph and create matrix
		if (args.length < 1) {
			System.out.println("USAGE: Chase #rooms #room1 #room2 #distance ...");
			System.exit(1);
		}
		if (((args.length - 1) % 3) != 0) {
			System.out.println("Must give descriptions of vertices as #room1 #room2 #distance");
			System.exit(1);
		}
		int n = Integer.parseInt(args[0]);
		graph = new int[n][n];
		
		//We Fill the Graph edge representation with the length of the edge between v and w,
		//where v and w are the room number.
		for (int i = 1; i < args.length; i+= 3) {
			v = Integer.parseInt(args[i]);
			w = Integer.parseInt(args[i + 1]);
			d = Integer.parseInt(args[i + 2]);             
			graph[v][w] = d;
			graph[w][v] = d;
		}		
		
		//Find shotest paths (Exit = room 0, Borogrove = room 1, Snark = room 2)
		Vertice[] shortestPaths = shortestPath(graph, 0);
		
		/*
		 * Compute the shortest distance to each of the points along the borogrove's route, 
		 * and calculate how long it takes the snark to get to each of those points and how long
		 * it takes for the borogrove to reach it, and compare the values?
		 */
		
		//Trace the path the borogrove takes.
		int pos = 1;	//The position of the borogrove.
		int id = 0;
		int[] borogrovePath = new int[n];
		while (pos != 0) {
			borogrovePath[id] = shortestPaths[pos].getId();
			pos = shortestPaths[pos].getParent();
			id += 1;
		}


		Vertice[] borogroveOptions = shortestPath(graph, 1);
		Vertice[] snarkOptions = shortestPath(graph, 2);
		
		
		//Check each of these points whether the snark can get there before the borogrove.
		int snarkD = Integer.MAX_VALUE;
		int borogroveD = Integer.MAX_VALUE;
		boolean eats = false;
		for (int p : borogrovePath) {
			snarkD = snarkOptions[p].getDist();
			borogroveD = borogroveOptions[p].getDist();
			if ((snarkD / snarkV) < (borogroveD / borogroveV)) {
				eats = true;
				break;
			}
		}
		
		if (eats) {
			System.out.println("Snark eats.");
		} else {
			System.out.println("Borogrove escapes.");
		}				
	}
	
	/**
	 * Dijkstra's Algorith for finding the shotest paths to all nodes from a given node.
	 * @param graph Graph in the form of a matrix, with edge weights as values of points in matrix (0 meaning no edge)
	 * @param start Node to start at
	 * @return Array of vertices indexed according to vertice ID, with distance value and parent value the appropriate values for a traversal from the start node.
	 */
	private static Vertice[] shortestPath(int[][] graph, int start) {
		PriorityQueue<Vertice> fringe = new PriorityQueue<Vertice>(graph.length);
		Vertice[] vertices = new Vertice[graph.length];
		
		for (int v = 0; v < graph.length; v+= 1) {
			vertices[v] = new Vertice(v, Integer.MAX_VALUE, -1);
			fringe.add(vertices[v]);
		}
		fringe.remove(vertices[start]);
		vertices[start].setDist(0);	
		fringe.add(vertices[start]);
		
		while (!fringe.isEmpty()) {
			Vertice v = fringe.remove();
			for (int i = 0; i < graph.length; i++) { 
				if ((graph[v.getId()][i] != 0) && ((v.getDist() + graph[v.getId()][i]) < vertices[i].getDist())) {
					fringe.remove(vertices[i]);
					vertices[i].setDist(v.getDist() + graph[v.getId()][i]);
					vertices[i].setParent(v.getId());
					fringe.add(vertices[i]);
				}
			}
		}
		return vertices;
	}
	
	/**
	 * Represents a single Vertice in a spanning graph from a certain starting node.
	 * Stores ID, distance and parent values (integers) to indicate position in the
	 * minimum graph.
	 */
	static class Vertice implements Comparable<Vertice> {
		
		private int dist;
		private int id;
		private int parent;
		private boolean marked;
		
		Vertice(int id, int dist, int parent) {
			this.id = id;
			this.dist = dist;
			this.parent = parent;
			marked = false;
		}
		
		int getDist() {
			return dist;
		}
		
		void setDist(int dist) {
			this.dist = dist;
		}
		
		int getId() {
			return id;
		}
		
		int getParent() {
			return this.parent;
		}
		
		void setParent(int parent) {
			this.parent = parent;
		}
		
		void mark() {
			marked = true;
		}
		
		void unMark() {
			marked = false;
		}
		
		boolean isMarked() {
			return marked;
		}
		
		public String toString() {
			return "" + id + " " + dist + " " + parent;
		}
		
		public int compareTo(Vertice b) {
			if (b.dist > dist) return -1;
			if (b.dist == dist) {
				if (b.id > id) return -1;
				if (b.id == id) return 0;
				if (b.id < id) return 1;
			}
			return 1;
		}
		
	}
	
}