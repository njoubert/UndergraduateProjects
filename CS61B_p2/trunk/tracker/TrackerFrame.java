package tracker;

import java.awt.BorderLayout;

import javax.swing.JFrame;

public class TrackerFrame extends JFrame {

	TrackerFrame(String title, State theState) {
		this.theState = theState;
		
		setDefaultCloseOperation (EXIT_ON_CLOSE);
		setDefaultLookAndFeelDecorated(true);
		
		java.awt.Toolkit.getDefaultToolkit().setDynamicLayout(true);
		board = new TrackerBoard(theState);
		
		getContentPane ().add (board, BorderLayout.CENTER);
		
		pack ();
		setVisible (true);
	}
	
	public void repaint() {
		try { Thread.currentThread().sleep(50);
		} catch (Exception e) {
		}
		board.repaint();
	}
	
	public void setState(State theState) {
		this.theState = theState;
		board.setState(theState);
	}
	
	private TrackerBoard board;
	private State theState;
}
