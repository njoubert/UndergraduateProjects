package game;

import java.awt.Dimension;
import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Color;
import javax.swing.JComponent;

import ucb.util.mailbox.Mailbox;

@SuppressWarnings("serial") class JumpingCubeBoard extends JComponent implements MouseListener {
	
	static final int 
	SQUARE_SIZE = 40,
	LINE_WIDTH = 1;
	
	private int sideOfBoard () {
		return myBoard.size () * SQUARE_SIZE + LINE_WIDTH;
	}
	
	/** A new displayable board that sends moves (in the form of strings
	 *  of the form 'row:col') to MOVEPORT. */
	JumpingCubeBoard (Mailbox<String> movePort) {
		this.movePort = movePort;
		myBoard = new Board (6);
		int side = sideOfBoard ();
		setPreferredSize (new Dimension (side, side));
		addMouseListener (this);
		acceptingMoves = false;
	}
	
	/** The method called by the screen-drawing thread to draw the current
	 *  state of THIS.   It eventually gets called whenever my window is
	 *  is displayed---when it is first displayed, when it is re-opened after
	 *  being iconized, and when an obscuring window is moved from on top of
	 *  it.   It is never called directly by the program.
	 *  KEEP IN MIND:
	 *  A Row value corresponds to some value on the y-axis, and a
	 *  Column value corresponds to some value on the x-axis.  */
	public synchronized void paintComponent (Graphics g0) {
		Graphics2D g = (Graphics2D) g0;
		int 
		N = myBoard.size (), 
		side = sideOfBoard ();
		setPreferredSize (new Dimension (side, side));
		g.setPaintMode ();

		//Fill the background (which will become the grid)
		g.setColor(gridColor);
		g.fillRect(0, 0, side, side);
		
		//Paint the Board.
		for (int r = 1; r <= N; r += 1) {
			for (int c = 1; c <= N; c += 1) {
				
				//Paint the Sqaure
				g.setColor(myBoard.color(r, c).toAWTColor());
				g.fill3DRect((c - 1) * SQUARE_SIZE + LINE_WIDTH, 
						(r - 1) * SQUARE_SIZE + LINE_WIDTH, 
						SQUARE_SIZE - LINE_WIDTH, 
						SQUARE_SIZE - LINE_WIDTH, true);
				
				//Paint the spots
				paintSpots(g, 
						(c - 1) * SQUARE_SIZE + LINE_WIDTH,
						(r - 1) * SQUARE_SIZE + LINE_WIDTH,
						myBoard.spots(r, c));
			}
		}
	}
	
	/** Paints the amount of given spots 
	 * in the square with upper left coordinate (x, y) */
	void paintSpots(Graphics2D g, int x, int y, int spots) {
		int side = SQUARE_SIZE/8;
		int start = (SQUARE_SIZE / 4);
		g.setColor(spotColor);
		switch (spots) {
		case 1: {
			g.fillOval(x + (SQUARE_SIZE/2) - side/2, y + (SQUARE_SIZE/2) - side/2, side, side);
			break;
		}
		case 2: {
			g.fillOval(x + start, y + start, side, side);
			g.fillOval(x + SQUARE_SIZE-start-side, y + SQUARE_SIZE-start-side, side, side);
			break;
		}
		case 3: {
			g.fillOval(x + start, y + SQUARE_SIZE-start-side, side, side);
			g.fillOval(x + (SQUARE_SIZE/2) - side/2, y + (SQUARE_SIZE/2) - side/2, side, side);
			g.fillOval(x + SQUARE_SIZE-start-side, y + start, side, side);
			break;
		}
		case 4: {
			g.fillOval(x + start, y + start, side, side);
			g.fillOval(x + SQUARE_SIZE-start-side, y + start, side, side);
			g.fillOval(x + start, y + SQUARE_SIZE-start-side, side, side);
			g.fillOval(x + SQUARE_SIZE-start-side, y + SQUARE_SIZE-start-side, side, side);
			break;
		
		}
		default: {
			g.drawString("" + spots, x + (SQUARE_SIZE / 2) - 4, y + (SQUARE_SIZE / 2) + 4);
			break;
		}
		}
	}
	
	/** Set my internal copy of the playing board to a copy of BOARD.  
	 *  Called by the game engine (the main thread). */
	synchronized void setBoard (Board board) {
		myBoard = new Board (board);
		repaint ();
	}
	
	/** Enable input from the mouse iff ENABLE */
	synchronized void enableMouseInput (boolean enable) {
		this.acceptingMoves = enable;
	}
	
	/* Methods to handle the mouse (see MouseListener).  Because of the call
	 * to addMouseListener in the constructor, all mouse events call one
	 * of these.  They are synchronized because the event thread, rather than
	 * the main thread, calls them, both may need the same data.   */
	
	/** Respond to a button clicked in my window.  If we are accepting moves,
	 *  and this is a valid move, sends it to movePort, and disables further
	 *  input. */
	public synchronized void mouseClicked(MouseEvent e) {
		if (! acceptingMoves)
			return;
		int x, y, r = 0, c = 0, button;

		x = e.getX ();
		y = e.getY ();

		button = e.getButton ();    
		
		if (button == 1) {
			c = x / SQUARE_SIZE + 1;
			r = y / SQUARE_SIZE + 1;
			try {
				movePort.deposit(r + ":" + c);
			} catch (Exception a) { 
				Main.exit(1);
			}
		}
		
	}
	
	/** We do nothing in response to just pressing the mouse. */
	public synchronized void mousePressed(MouseEvent e) {}
	/** We do nothing in response to just releasing the mouse. */
	public synchronized void mouseReleased(MouseEvent e) {}
	/** We do nothing special when the mouse enters our window. */
	public synchronized void mouseEntered(MouseEvent e) {}
	/** We do nothing special when the mouse leaves our window. */
	public synchronized void mouseExited(MouseEvent e) {}

	
	/* End of mouse-handling methods */
	
	/** A private copy of the board I am displaying.  We keep it separate from
	 *  the game to avoid race conditions. */
	private Board myBoard;
	/** The place in which I am to put all the moves entered by the mouse. */
	private Mailbox<String> movePort;
	/** True if we are accepting moves from the screen at this point. */
	private boolean acceptingMoves;
	
	/** Color of grid. */
	private Color gridColor = new Color (0, 0, 0);
	/** Color of the spots. */
	private Color spotColor = new Color (0, 0, 0);
	
}

