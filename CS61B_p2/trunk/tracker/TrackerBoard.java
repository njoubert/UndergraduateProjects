package tracker;

import java.awt.Dimension;
import java.awt.Graphics;

import javax.swing.JComponent;

public class TrackerBoard extends JComponent {
	
	TrackerBoard(State theState) {
		this.theState = theState;
		llx = theState.llx(); lly = theState.lly(); urx = theState.urx(); ury = theState.ury();
		if ((urx-llx) < 50 || (ury-lly) < 50) BIGGER = 5;
		myDim = new Dimension (((int) (ury-lly+1))*BIGGER + 2, ((int) (urx-llx+1))*BIGGER + 2);
		setPreferredSize (myDim);
	}
	
	public void setState(State theState) {
		this.theState = theState;
		repaint();
	}
	
	public synchronized void paintComponent (Graphics g0) {
		int id;
		util.Set2DIterator points = theState.iterator();
		//g0.setColor(java.awt.Color.WHITE);
		//g0.fillRect(0, 0, myDim.width, myDim.height);
		g0.setColor(java.awt.Color.BLACK);
		while (points.hasNext()) {
			id = points.next();
			g0.fillOval(convertX(theState.getX(id)), convertY(theState.getY(id)), ((int) (theState.getRadius() + 1))+1, ((int) (theState.getRadius() + 1))+1);
		}
		
	}
	
	//Convert a point that is relative to the coordinate axis to a value from 0 (where 0 is left)
	private int convertX(double x) {
		return ((int) (Math.abs(llx) + x)) * BIGGER + 1;
	}
	
	private int convertY(double y) {
		return ((int) Math.abs(ury - y)) * BIGGER + 1;
		
	}
	
	private int BIGGER = 2;
	private State theState;
	private double llx, lly, urx, ury;
	private Dimension myDim;
}
