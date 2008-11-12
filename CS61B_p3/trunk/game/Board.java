package game;

import java.util.Formatter;
import java.util.ArrayList;
import static game.Color.*;

/** A Jump61b board state.  
 * 
 * 
 * <H3>THE BOARD</H3>
 * <P>
 * A board is a grid of N rows and N columns, 
 * where (1 <= r, c <= N)
 * 
 * To represent the board we need some data structure that holds
 * N*N points, each one with a color and number of dots associated.
 * The number of dots vary between one and five (although during operation
 * this might be exceeded in the middle of movement procedures), and 
 * colors (as represented in the Color enumeration) as White, Blue and Red.
 * 
 * <P>The most obvious choices for representing this structure are
 * either an OO-approach, using an object for each square and
 * building a "grid" of these objects. By using fake nodes
 * as the borders of the board, all squares will have
 * 4 neighbors (the borders and corners has some virtual ones)
 * and, by using return values correctly and being careful in
 * writing base cases in the nodes, we can build this.
 * <BR><u>The advantage of this</u> is the clean code (especially the 
 * jumping of spots) and the object-orientated approach.
 * <br><u>The disadvantage of this</u> is the space inefficiency over some
 * other methods and the limitations of Java's non-tail-recursive implementation.
 * I found that my OO-approach was limited to a small number of jumps, insufficient
 * for the size of boards I needed, thus I abandoned this approach (its still some
 * separate branch in my CVS repository if i need it again hehehe).
 * <P>A N-by-N array can also be used to represent the matrix.
 * By building a data abstraction layer for the squares that
 * encodes squares as byte values, we can use this array
 * of rows and columns exactly like a board through some
 * streamlined getter and setter methods.
 * <BR><u>The advantage of an array</u> to represent this matrix, with some smart math and bitshifting
 * behind it, is its small size, incredibly fast speed if we stay away from recursion, 
 * and it is fun to code, seeing that I enjoy incorporating some math into my programs.
 * This also allows for some creative encoding schemes for the
 * values of squares.
 * </P>
 * <P>Needless to say, I chose the latter.
 * The whole board is represented as an N by N array of
 * bytes, where the first N array represents the rows
 * and the second R array represents the columns in
 * each row.
 * To retrieve a certain row and certain column from
 * the board you simply use
 * myBoard[r-1][c-1]
 * 
 * The reason for the negatives is that the array reaches
 * from 0 to (n-1) while input ranges from 1 to n.
 * 
 * <BR>I streamline access to the board through getter and setter procedures
 * (getSquareAt,setSquareAt,incrementSquareAt,decrementSquareAt, withinBounds) which is used by
 * all the other methods in this class to access the board.
 * </P>
 * <P><H2>DATA ABSTRACTIONS IN THE BOARD CLASS</H2>
 * <P>There are a couple of data abstractions in this class:
 * <ul><li>Square - a byte representation of a color, spots value.</li>
 * <li>Position - a int array representing a {r,c} value</li>
 * <li>Board - a byte 2D array representing a board as byte[r][c] values</li>
 * </ul>
 * <BR>==================== BOARD DATA ABSTRACTION ==================== 
 * <BR>These methods end with "SquareAt" seeing that we
 * work with the square at a certain position on
 * the board.
 * <BR>==================== POSITION ====================
 * <BR>Uses the Position Data Abstraction:
 * A positon is represented by a legnth 2 array of integers,
 * with the first value representing the row number, and the
 * second value representing the column number.
 * <BR>==================== SQUARE DATA ABSTRACTION ==================== 
 * <BR>All methods contain "ASquare"
 * seeing that these methods work with a single square as
 * both input and output.
 * 
 * 
 * <H3>EACH SQUARE OF THE BOARD</H3>
 * <P>
 * A byte represents one square, encoding a color and number of spots.
 * The byte value encodes values as this:
 * 
 * <BR>byte value = ((color.ordinal() << 5) ^ (spots - 1));<BR>
 * 
 * Thus to decode the byte values:
 * 
 * <BR>color = (square >>> 5)
 * <BR>spot nr = (square & 0x1f) + 1
 * <BR>
 * The big idea here is to compress the color value (by its ordinal value)
 * and the number of spots into a single byte.
 * The reasoning behind the minus one, plus one here is to 
 * dissuade from the necessity to "instantiate" the new array
 * into certain values beforehand. By default any zero value
 * is decoded as a "White, 1 spot" value.
 * 
 * <BR>Access to all of this is given through getter and ctreator procedures\
 * (getASquareSpots, getASquareColor, makeASquare)
 * </P>
 * <H3>LEGAL MOVES</H3>
 * <P>
 * A move can only be legal if there are any legal moves on the board,
 * and that the specific point is a legal square.
 * Adding to a square is a legal move if the square is
 * not the color of the opponent. 
 * 
 * In describing legal moves and moves in general there are some issues
 * that must be addressed. If you have a legal move, it means that
 * there is still a square that is your color and a square that is your
 * opponent's color. Thus, if all the squares are the same color it is illegal for both.
 * You can traverse the whole board to check this, but it might not be the
 * best approach.
 * <P> I hold an invatriant of the total number of squares of each color
 * on the board, so thagt at any moment i have a counter value of the number
 * of white, red and blue squares, where they always add up to the total number
 * of squares on the board. This stremlines isLegal and getWinner and makes it possible to
 * check at every iteration of the spot jumping procedures whether we have a winner
 * yet or not.
 * </P>
 * <H3>ADDING SPOTS TO A BLOCK</H3>
 * <P>
 * We assume that the block is a legal place.
 * The first step is to add a spot and color the block
 * the color of the current player.
 * This is done through the encoding algorithm described above.
 * Now we need to start jumping spots...
 * </P>
 * 
 * <H3>SPOT MOVEMENT</H3>
 * <P>
 * Once a spot has been added, spots needs to be 
 * distributed so that the equilibrium state of the
 * board is again reached.
 * This simple rule is that no square can have more
 * spots than neighbors, if it has, the spots
 * is distributed to these nieghbors, one per neighbor.
 * There are three cases that needs to be handled in 
 * this situation - inner squares, border squares and
 * corner squares. These can all be handled
 * simply by looking at each neighbor one by one, and if
 * there is one, we take the appropriate action to increment it
 * and decrement the current spot. 
 * <br>This is the algorithm for moving spots:
 * 
 * <br>1) increment the current spot. Change the color invariant accordingly.
 * <br>2) Add it to the queue of spots that has been changed.
 * <br>3) Check whether it is overfull, if it is:
  *<br> 3a) check the type of spot, and increment/decrement the spots accordingly (keeping the color invariant)
 * <br>4) Repeat the procedure with all the affected squares now in the queue.

 * </P>
 * <H3>COLOR VALUES INVARIANT</H3>
 * <P>To keep track of the amount of squares of each color on the board,
 * we keep a loop invariant of the amount of colors on the board:
 * <br><b># Red Squares + # Blue Squares + # White Squares = size() * size()</b>
 * <br>This needs to upheld by the spot adding/jumping procedure.
 * <br>These values are all stored in an array that stores the amount of squares
 * associated with a specific value in the indice corresponding to the ordinal value of
 * that color. The array is called <b>colorInvariant[]</b>  
 * Keeping the loop invariant is done mainly in the jumpNeighbors procedure. Every time
 * we change a square, we decrement the total number of spots with its original
 * color and increate the total number of squares of its new color.
 * AddSpot also contains this procedure for the moment we add a spot to the board.
 * <br>This allows us to stop the jumping the moment one player wins by owning the whole board.
 * 
 * </P>
 * <H3>ERROR HANDLING</H3>
 * <P>
 * If any command gets parameters that does not follow its expected valued,
 * it throws an IllegalArgumentException as expected.
 * <BR>The assertWithinBounds(r,c) method checks to see whether the given row-col value
 * is within the boundaries of the board, and if not it throws an IllegalArgumentException.
 * 
 * */
class Board {
	
	/** An NxN board in initial configuration. */
	Board (int N) {
		if (N < 2) throw new IllegalArgumentException("You cannot create a board smaller than 2-by-2.");
		myBoard = new byte[N][N];
		n = N;
		colorInvariant = new int[Color.values().length];
		colorInvariant[Color.WHITE.ordinal()] = n*n;
		colorInvariant[Color.RED.ordinal()] = 0;
		colorInvariant[Color.BLUE.ordinal()] = 0;
	} 
	
	/** A board whose initial contents are copied from BOARD0. */
	Board (Board board0) {
		myBoard = board0.myBoard.clone();
		for (int i = 0; i < board0.myBoard.length; i++) {
			myBoard[i] = board0.myBoard[i].clone();
		}
		n = board0.n;
		colorInvariant = board0.colorInvariant.clone();
	}
	
	/** The number of rows and of columns of THIS.
	 * i.e. 1 <= row <= size()
	 * and the same goes for columns */
	int size () {
		return n;
	}
	
	/** The number of spots in the square at row R, column C, 
	 *  1 <= R, C <= size (). Checks whether the input lies within the boundaries, 
	 *  then use the described algorithm to extract the spot number. */
	int spots (int r, int c) {
		assertWithinBounds (r, c);
		return getASquareSpots(getSquareAt(r, c));
	}
	
	/** The color of the square at row R, column C, 1 <= R, C <= size(). 
	 * Checks whether the input lies within the boundaries, 
	 *  then use the described algorithm to extract the color */
	Color color (int r, int c) {
		assertWithinBounds (r, c);
		return getASquareColor(getSquareAt(r, c));
	}
	
	/** True iff it is currently legal for PLAYER to add a spot to square 
	 *  at row R, column C. */
	boolean isLegal (Color player, int r, int c) {
		try { 
			assertWithinBounds (r, c);
		} catch (IllegalArgumentException e) {
			return false;
		}
		if (! isLegal (player)) return false;
		else if (color(r, c) != player.opposite()) return true;
		else return false;
	}
	
	/** True iff player with given Color is allowed to make some move at this
	 *  point.
	 *  This is only satisfied when the current player does not own the board
	 *  or the current player's opponent does not own the board. */
	boolean isLegal (Color player) {
		if ((colorInvariant[Color.RED.ordinal()] == 0 || colorInvariant[Color.BLUE.ordinal()] == 0) && 
				(colorInvariant[Color.WHITE.ordinal()] == 0)) return false;
		return true;
	}
	
	/**
	 * This physically counts every square to check the board - almost "backup check" for the color invariant.
	 * @return True iff the board has no more space for any squares.
	 */
	boolean isFull() {
		int spotsOnBoard = 0;
		for (int r = 1; r <= size(); r++) {
			for (int c = 1; c <= size(); c++) {
				spotsOnBoard += spots(r, c);
			}
		}
		int maxSpots = (n*n)*4 - 4*(n);
		return (spotsOnBoard == maxSpots);
		
	}
	
	/** The winner (if any) of the current position, if the game is over,
	 *  and otherwise null. Simply checks the color invariant to
	 *  see whether there the whole board is only one color, and
	 *  if it is, that person is returned as the winner, else
	 *  a null value is returned.
	 *  */
	Color getWinner (Color currentPlayer) {
		if (!isLegal(currentPlayer)) 
			if (colorInvariant[Color.RED.ordinal()] > colorInvariant[Color.BLUE.ordinal()]) {
				return RED;
			} else return BLUE;
		return null;
		
	}
	
	int hasSquares(Color player) {
		return colorInvariant[player.ordinal()];
	}
	
	/* ==================== JUMPING THE SPOTS ====================
	 * Uses the Position Data Abstraction:
	 * A positon is represented by a legnth 2 array of integers,
	 * with the first value representing the row number, and the
	 * second value representing the column number.
	 * 
	 */
	
	/** Add a spot from PLAYER at row R, column C.  Assumes 
	 *  isLegal (player, r, c).
	 *  <BR>A simple procedure of adding to a square, then iterating through
	 *  each affected square, checking whether it is overfull - if so, we use
	 *  the decreaseAndJump method to redistribute its spots.
	 *  We also keep the color invariant of the board:
	 *  # Red Squares + # Blue Squares + # White Squares = size() * size();
	 */
	void addSpot (Color player, int r, int c) {
		assertWithinBounds (r, c);
		if (color(r, c) == player.opposite()) throw new IllegalArgumentException("The specific spot it not of a color you can add to.");	
		ArrayList<int[]> queue = new ArrayList<int[]>();
		int[] pos = new int[2];
		pos[0] = r; pos[1] = c;
		
		colorInvariant[player.ordinal()] += 1;
		colorInvariant[color(r, c).ordinal()] -= 1;
		queue.add(incrementSquareAt(player, r, c));
		
		while (!queue.isEmpty() && isLegal(player)) {
			pos = queue.remove(0);
			if (isOverfull(pos)) {
				decreaseAndJump(pos, queue, player);
			}
		}
	}
	
	/**
	 * Used by addSpot to decrease the spots at and twiddle the spots around
	 * a certain position. This is achieved by looking at each neighbor and
	 * adding a spot to it if there is a neighbor, and keeping track of the amount
	 * of neighbors you've given spots to, and finally decrementing the current
	 * spot by this value. 
	 * Keeping the loop invariant is done mainly in this procedure. Every time
	 * we change a spot, we decrement the total number of spots with its original
	 * color and increate the total number of spots of its new color.
	 * @param pos - a position array {R,C} that is to be jumped from.
	 * @param queue - the queue which the loop is using to jump all the spots with.
	 * @param player - the color we are jumping with.
	 */
	private void decreaseAndJump(int[] pos, ArrayList<int[]> queue, Color player) {
		int decreaseBy = 0;
		if (pos[0] > 1) { 
			if (color(pos[0]-1, pos[1]).ordinal() != player.ordinal()) {
				colorInvariant[player.ordinal()] += 1;
				colorInvariant[color(pos[0] - 1, pos[1]).ordinal()] -= 1;
			}
			queue.add(incrementSquareAt(player, pos[0] - 1, pos[1]));
			decreaseBy += 1;
		}
		if (pos[0] < size()) {
			if (color(pos[0]+1, pos[1]).ordinal() != player.ordinal()) {
				colorInvariant[player.ordinal()] += 1;
				colorInvariant[color(pos[0] + 1, pos[1]).ordinal()] -= 1;
			}
			queue.add(incrementSquareAt(player, pos[0] + 1, pos[1]));
			decreaseBy += 1;
		}
		if (pos[1] > 1) {
			if (color(pos[0], pos[1]-1).ordinal() != player.ordinal()) {
				colorInvariant[player.ordinal()] += 1;
				colorInvariant[color(pos[0], pos[1] - 1).ordinal()] -= 1;
			}
			queue.add(incrementSquareAt(player, pos[0], pos[1] - 1));
			decreaseBy += 1;
		}
		if (pos[1] < size()) {
			if (color(pos[0], pos[1]+1).ordinal() != player.ordinal()) {
				colorInvariant[player.ordinal()] += 1;
				colorInvariant[color(pos[0], pos[1] + 1).ordinal()] -= 1;
			}
			queue.add(incrementSquareAt(player, pos[0], pos[1] + 1));
			decreaseBy += 1;
		}
		decrementSquareAt(pos[0], pos[1], decreaseBy);
	}
	
	/**
	 * Checks to see whether the given square is overfull.
	 * This is accomplished by calculating the maximum number of spots
	 * allowed at a given position, and checking whether the given position
	 * exceeds this number.
	 * <br>We know that all squares can keep 2 spots at least, so we set this
	 * as a starting value for the maximum amount of spots. We then check whether
	 * its in the top or bottom row, if not, increase this value by one. We do
	 * the same for the left/right row.
	 * @param pos - a position array of the form {R,C}
	 * @return true if the given spot is overfull.
	 */
	private boolean isOverfull(int[] pos) {
		int maximumAllowedSpots = 2;
		if (pos[0] != 1 && pos[0] != size()) maximumAllowedSpots += 1;
		if (pos[1] != 1 && pos[1] != size()) maximumAllowedSpots += 1;
		if (getASquareSpots(getSquareAt(pos[0], pos[1])) > maximumAllowedSpots) return true;
		return false;
	}
	
	/** The dumped representation of THIS. */
	public String toString () {
		Formatter out = new Formatter ();
		out.format("===");
		for (int r = 1; r <= size(); r++) {
			out.format("\n    ");
			for (int c = 1; c <= size(); c++) {
				if (color(r,c) == WHITE) out.format("-- ");
				else out.format(spots(r,c) + color(r,c).toStringSmall() + " ");
			}
		}
		out.format("\n===");
		return out.toString ();
		
	}
	
	/** An external rendition of THIS, suitable for human-readable textual 
	 *  display.  This is intended to be distinct from the dumped (toString)
	 *  representation. */
	public String toDisplayString () {
		Formatter out = new Formatter ();
		out.format("===");
		for (int r = 1; r <= size(); r++) {
			out.format("\n    ");
			for (int c = 1; c <= size(); c++) {
				if (color(r,c) == WHITE) out.format("-- ");
				else out.format(spots(r,c) + color(r,c).toStringSmall() + " ");
			}
		}
		out.format("\n===");
		return out.toString ();
	}
	
	
	
	/* ==================== BOARD DATA ABSTRACTION ==================== 
	 * These methods end with "SquareAt" seeing that we
	 * work with the square at a certain position on
	 * the board. */
	
	/** The Board represented as an 2D array of position data. */
	private byte[][] myBoard; 	//The board representation
	/** The number of rows or columns on the board*/
	private int n; 				//The number of rows or columns
	/** The color invariant storage space.
	 *  Stores the invariant: redSquares + blueSquares + whiteSquares = size() * size()*/
	private int colorInvariant[]; 
	
	/**
	 * Extracts the square with the given row, column position
	 * from the board.
	 * @return the byte square representation as described by the Square Data Abstraction
	 */
	private byte getSquareAt(int r, int c) {
		return myBoard[r-1][c-1];
	}
	
	/**
	 * Sets the value of the given position on the board to the given square.
	 * @param square - the byte square representation as described by the Square Data Abstraction
	 * @param r - the row value, 1 <= r <= n
	 * @param c - the col value, 1 <= c <= n
	 */
	private void setSquareAt(byte square, int r, int c) {
		myBoard[r-1][c-1] = square;
	}
	
	/**
	 * Increments the amount of spots of the square at the given row, col position.
	 * This also involves changing it to the given color value.
	 * @param color - the color of the square after incrementing it.
	 * @param r - the row value, 1 <= r <= n
	 * @param c - the col value, 1 <= c <= n
	 * @return position - a 2-length array representing the {row, col} position.
	 */
	private int[] incrementSquareAt(Color color, int r, int c) {
		setSquareAt(makeASquare(color, getASquareSpots(getSquareAt(r,c)) + 1), r, c);
		int[] pos = new int[2];
		pos[0] = r; pos[1] = c;
		return pos;
	}
	
	/**
	 * Decrements the amount of spots of the square at the given row, col position,
	 * with the given amount.
	 * @param r - the row value, 1 <= r <= n
	 * @param c - the col value, 1 <= c <= n
	 * @param amount - the amount with which to decrese the square
	 */
	private void decrementSquareAt(int r, int c, int amount) {
		setSquareAt(makeASquare(getASquareColor(getSquareAt(r,c)), getASquareSpots(getSquareAt(r,c)) - amount), r, c);
	}
	
	/** 
	 * Checks whether the given position is within the boundaries
	 * of the current board. Throws an IllegalArgumentException with an appropriate
	 * message if the given point is not inside the size of the board.
	 * @param r
	 * @param c
	 */
	private boolean assertWithinBounds(int r, int c) {
		if (r < 1 || c < 1 || r > n || c > n) throw new IllegalArgumentException("The specific R:C value is outside the board.");;
		return true;
	}
	
	/* ==================== SQUARE DATA ABSTRACTION ==================== 
	 * All methods contain "ASquare"
	 * seeing that these methods work with a single square as
	 * both input and output. */
	
	/**
	 * @return The amount of spots encoded in the given square byte.
	 */
	private static int getASquareSpots(byte square) {
		int returnVal = 0;
		returnVal = (square & 0x1f) + 1;
		return returnVal;
	}
	
	/**
	 * @return The color of the square encoded in the given square byte.
	 */
	private static Color getASquareColor(byte square) {
		int returnVal = 0;
		returnVal = (square >>> 5);
		return Color.parseColor(returnVal);
	}
	
	/**
	 * Creates a square according to the given paramteres
	 * @param color - The color that will be encoded onto the square.
	 * @param spots - The amount of spots to be encoded on the square.
	 * @return The paramteres encoded into a byte square value.
	 */
	private static byte makeASquare(Color color, int spots) {
		byte returnVal = 0;
		returnVal = ((byte) ((color.ordinal() << 5) ^ (spots - 1)));
		return returnVal;
	}
}
