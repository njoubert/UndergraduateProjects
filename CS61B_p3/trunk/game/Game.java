/* Niels Joubert CS61b-bo*/

/* This is the main Game driver */

package game;

import static game.Color.*;
import static game.Game.State.*;
import java.util.Scanner;
import java.io.*;

/** 
 * <P>Represents the state of a game of Jump61b. 
 * </P>
 * <H2>Black-Box Specifications</H2>
 * <P> 
 * 
 * </P>
 * <table width=100% border=1><tr><td>
 * <H4>Modes</H4>
 * <P>Defined by the source of moves.
 * <BR><ul>
 * <li><B>Local Mode</B> - Player 2 is played by this program.</li>
 * <li><B>Host Mode</B> - Player 2 is taken by the remote program, and we host the game, thus we can define and set up the board during set-up state. </li>
 * <li><B>Client Mode</B> - Player 2 is taken by the remote program, which serve as the host, and we have no control over the set-up stage. </li>
 * </ul>
 * </P>
 * </td></tr></table>
 * <table width=100% border=1><tr><td>
 * <H4>States</H4>
 * <P>Defined by the progression of the game.
 * <BR><ul>
 * <li><B>Set-up State</B> - any move added only changes the initial board configuration.</li>
 * <li><B>Playing State</B> - normal playing, where the request for a move is alternated between the two players.</li>
 * <li><B>Finished State</B> - a player one and no more moves are possible.</li>
 * </ul>
 * </td></tr></table>
 * <P>Initially the board is in the <u>local mode</u> and <u>set-up state</u>
 * 
 * </P>
 * <H3>'Add-ons' for reading input</H3>
 * <P>Since certain players (HumanPlayer notably) reads input from the 
 * terminal, but we never want to close the Scanner that uses the
 * System.in input, we create the Scanner in the Game class, and allow
 * the Players to access this Scanner as necessary. The Scanner also
 * uses the required pattern as delimiter to remove comments:
 * To remove comments and retrieve a whole line at one time, we set up a delimiter that
 * matches comments (optionally) and newlines (that corresponds to windows and *nix standards.)
 * which looks like this: "(#.*)?(\\r)?\\n" 
 * </P>
 * <H3>Keeping track of turns and current move's color/player</H3>
 * <P>
 * RED moves first!
 * <br>
 * Since moves (commands of the form R:C) puts a certain <i>color</i> of spot on the board,
 * depending on which state we are in and which player is issuing the color, and since
 * this also determined from which player we are reading, we need to keep track of this somehow.
 * <br>Since the primary distinguishment between a move's color is, well, its color, and not
 * necessarily the player is comes from, the underlying idea is to hold a variable representing
 * the current move's color in the game object.
 * <br>We implement a data abstraction on top of this which uses this current value to
 * find the player, the color or the player's number (Player 1 or 2) according to the value
 * of the whoseMove color variable.
 * <br>When we need to know whose turn it is, we use our getter methods to access this variable.
 * <br>When we need to transfer control to the other player (or, more accurately put, ask for
 * the next command of the other player), we also use the getter methods to access this variable.
 * </P>
 * <h4>Differences between States</h4>
 * <P>
 * In <b>SET-UP state</b>, the color of a move is dependent on <i>when</i> the move is issued, 
 * that is, relative to the last command, what color is this command? According to 
 * the current color variable, the current color is!
 * <br>
 * In <b>PLAYING state</b>, the color of a moveis dependent on <i>the player's color</i> that
 * issues it - which must correspond to the color of the player we are reading input from.
 * <br>
 * In <b>FINISHED state</b>, no more move commands are possible, and they are thus ignored - color
 * is irrelevant, and does not change during or affect this mode.
 * </P>
 * <P>
 * <H3>Command Execution Methods</H3>
 * <P>All the command executions returns a string - a nice, friendly message to the 
 * player that issued the command. If something goes wrong, it throws an error (or simply
 * passes the error on) to the main Game class.
 * </P>
 * <H3>Error Handling</H3>
 * <P>All the command execution methods throw descriptive errors
 * which is caught inside the actual driverloop, which handles this
 * by sending the error to the player, and NOT switching control to the
 * other player if this we are playing the game. 
 * </P>
 * <H3>Expected Command</H3>
 * <P>
 * We use the expected command methods to handle situations where you must get a specific command
 * from a certain player before the game can continue.
 * Also, in remote mode, it'll handle illegal messages recieved.
 * </P>
 * 
 * */
class Game {
	
	static enum State {
		SETUP, PLAYING, FINISHED
	}
	
	/** A Game in the initial configuration with one human and one automated
	 *  player, a cleared board, and in the SETUP state. */
	Game (Player player1) {
		board = new Board(6);
		player[0] = player1;
		player[1] = new MachinePlayer ();
		player[0].setColor (RED);
		player[1].setColor (BLUE);
		player[0].setGame (this);
		player[1].setGame (this);
		
		setTurn(RED);
		
		stdIn = new Scanner(System.in);
		stdIn.useDelimiter(INPUTPATTERN);
		
		state = SETUP;
	}
	
	/** The Player representing player #K (K=1 or 2).  Player #1 is the
	 *  human sitting at the terminal (or using a GUI) or an automated player 
	 *  that is moving for him.  Player #2 is an automated player (in 
	 *  a local game) or a remote player (host or client). */
	Player getPlayer (int k) {
		assert k == 1 || k == 2;
		return player[k-1];
	}
	
	/** The current state of play. */
	State getState () {
		return state;
	}
	
	boolean inRemoteMode() {
		return (getPlayer(2).type().equals("Host") || getPlayer(2).type().equals("Client"));
	}
	
	/** Returns the winner of the current game. 
	 * This is checked against the private winner variable to check whether someone has
	 * already won - for example, the remote player left the game, so you won - and if
	 * not, we query the board to check whether there is a winner yet.*/
	Color getWinner() {
		if (winner == null) {
			winner = board.getWinner(whoseTurnColor());
		}
		return winner;
	}
	
	/** Sets the winner of the current game.
	 * Useful for handling remote players that quits before the game is over. */
	void setWinner(Color winner) {
		this.winner = winner;
	}
	
	/** Clears the current winner state. */
	void clearWinner() {
		winner = null;
	}
	
	/** Play one or more games of Jump61b. */
	void play () throws ExitException {
		
		Command c = null;
		
		while (true) {
			
			c = null;
			
			try {
				
				if (whoseTurnInt() == 1) getPlayer(1).sendCommand("> ");	
				if (hasExpectedCommand()) {
					c = Command.create(getPlayer(expectedPlayer).nextCommand(), this, getPlayer(expectedPlayer), null, getPlayer(expectedPlayer).getColor());
					if (!checkExpectedCommand(c)) {
						if (inRemoteMode()) illegalRemoteOperation(c.type + " was recieved instead of the expected " + expectedCommand);
						else throw new IllegalArgumentException("A different command from the expected command " + expectedCommand + " was recieved.\nThe recieved command was " + c.type);
					}
					
				} else {
					if (getState() == SETUP && getPlayer(2).type().equals("Client")) {	
						c = Command.create(getPlayer(2).nextCommand(), this, getPlayer(2), getPlayer(1), whoseTurnColor() );	
					} else if (getState() == SETUP) {					
						c = Command.create(getPlayer(1).nextCommand(), this, getPlayer(1), getPlayer(2), whoseTurnColor() );					
					} else if (getState() == PLAYING) {								
						if (getWinner() != null) {
							state = FINISHED;
							displayWinner();
							if (inRemoteMode() && (getWinner() == getPlayer(1).getColor())) {
								setExpectedCommand("quit", 2);
							} else {								
								setExpectedCommand(null, 0);
								getPlayer(1).sendCommand("> ");
							}
							c = null;
						} else {
							c = Command.create(whoseTurnPlayer().nextCommand(), this, whoseTurnPlayer (), null, whoseTurnColor ());		
						}					
					} 
					if (getState() == FINISHED && !hasExpectedCommand()) {		
						c = Command.create(getPlayer(1).nextCommand(), this, getPlayer(1), getPlayer(2), getPlayer(1).getColor() );					
					}
				}
				
				if (c != null) {
					c.execute();
					c.report();
				}
				
			} catch (ExitException e) {
				if (inRemoteMode()) {
					try {
						getPlayer(2).sendCommand("quit");
						getPlayer(2).flush(1000);
					} catch (Exception a) { };
					clear();
				} else {
					throw new ExitException();
				}
			} catch (Exception e) {
				try { 
					setExpectedCommand(null, 0);
					getPlayer(1).error(e.getMessage() + "\n");
				} catch (Exception a) {
					System.out.println(a.getMessage() + "\n");
				}
			}
		}
	}
	
	
	/** Clear the board to its initial configuration and enter set-up state,
	 *  placing both Players in their initial configurations. */
	void clear () {
		clearWinner();
		board = new Board(board.size());
		player[1].close();
		player[1] = player[1].automatedPlayer();
		setTurn(RED);
		state = SETUP;
		isConnected = false;
		setExpectedCommand(null, 0);
	}
	
	/** A copy of the current game board. */
	Board getBoard () {
		return new Board (board);
	}
	
	/** Purely for testing! */
	void setBoard(Board b) {
		board = b;
	}
	
	/**Duplicates the standard input scanner's nextLine method.
	 * @return The next line in the standard input.
	 */
	public String stdInNextLine() {
		return stdIn.nextLine();
	}
	/**Duplicates the standard input scanner's next method.
	 * @return The next token (according to the delimited described above) in the standard input.
	 */
	public String stdInNext() {
		return stdIn.next();
	}
	
	/** The size of the current board. */
	int size () {
		return board.size ();
	}
	
	/** The number of spots in the square at row R, column C, 
	 *  1 <= R, C <= size (). */
	int spots (int r, int c) {
		return board.spots (r, c);
	}
	
	/** The color of the square at row R, column C, 1 <= R, C <= size(). */
	Color color (int r, int c) {
		return board.color (r, c);
	}
	
	/** True iff it is currently legal for PLAYER to add a spot to square 
	 *  at row R, column C. */
	boolean isLegal (Color player, int r, int c) {
		return board.isLegal (player, r, c);
	}
	
	/** True iff player with given Color is allowed to make some move at this
	 *  point. */
	boolean isLegal (Color player) {
		return board.isLegal (player);
	}
	
	/** Returns the Color of the player who must make the next move. */
	Color whoseTurnColor() {
		return whoseTurn;
	}
	
	/** Returns the Player who must make the next move. */
	Player whoseTurnPlayer() {
		if (getPlayer(1).getColor() == whoseTurn) return getPlayer(1);
		else return getPlayer(2);
	}
	
	/** Returns the Player who must NOT make the next move. */
	Player whoseNotTurnPlayer() {
		if (getPlayer(1).getColor() != whoseTurn) return getPlayer(1);
		else return getPlayer(2);
	}
	
	/** Returns the number of the player who must make the next move. */
	int whoseTurnInt() {
		if (getPlayer(1).getColor() == whoseTurn) return 1;
		else return 2;
	}
	
	/** Sets the Color of the next turn (next move) according to the Color of the player associated with the given number. */
	void setTurn(int player) {
		whoseTurn = getPlayer(player).getColor();
	}
	
	/** Sets the Color of the next turn (next move) according to the Color of the given Player. */
	void setTurn(Player player) {
		whoseTurn = player.getColor();
	}
	
	/** Sets the Color of the next turn (next move) according to the given Color. */
	void setTurn(Color color) {
		whoseTurn = color;
	}
	
	/** Sets the current turn (color of the next move) to the other player (other player's color). */
	void nextTurn() {
		whoseTurn = whoseTurn.opposite();
	}
	
	/*
	 * METHODS FOR EXECUTING COMMANDS FROM PARSERS
	 */
	
	void start() {
		if (state != SETUP) clear();
		state = PLAYING;
	}
	
	void game(int size, Color color) {
		board = new Board(size);
		player[0].setColor(color);
		player[1].setColor(color.opposite());
		setTurn(RED);
	}
	
	void auto() {
		player[0] = player[0].automatedPlayer();
	}
	
	void manual() {
		player[0] = player[0].manualPlayer();
	}
	
	void host(String id) throws IOException {
		
		player[1].close();
		player[1] = new HostPlayer(id);
		player[1].setGame(this);
		setExpectedCommand("clear", 2);
		
	}
	
	void join(String id, String host) throws IOException {
		
		player[1].close();
		player[1] = new ClientPlayer(host, id);
		player[1].setGame(this);
		player[1].sendCommand("clear");
		setExpectedCommand("game", 2);

	}
	
	/** Adds a spot (thus, makes the given move) on the board.
	 * Also switches to the next turn. */
	void move(int r, int c, Color color) {
		if (board.isLegal(color, r, c)) {
			board.addSpot(color, r, c);
			nextTurn();
		} else {
			throw new IllegalArgumentException(r + ":" + c + " is not a legal position for the " + color.toString() + " player.");
		}
		
	}
	
	void help() {
		System.out.println("Jump61B Help");
		System.out.println("-------------");
		System.out.println("Jump61B is an implementation of the popular KJumpingCube game.");
		System.out.println("The objective of the game is to fill a board with your own color");
		System.out.println("by placing spots in legal squares on a NxN board.");
		System.out.println("The board starts out initially with all white squares, one spot per square.");
		System.out.println("By putting a spot on a white square, it becomes the current player's color.");
		System.out.println("A square becomes overfull when it has more spots than neighbors (directly adjecent squares)");
		System.out.println("and the overfull square pushes out a spot to each of its neighbors.");
		System.out.println("This is the Jumping of Spots from which the name comes.");
		System.out.println("=== COMMANDS ===");
		System.out.println("clear\t\t\tAbandones the game, clears the board, start in set=up state.");
		System.out.println("start\t\t\texecutes clear if necessary, and starts game.");
		System.out.println("quit\t\t\tabandons current game and, if not in remote mode, quits the application.");
		System.out.println("game [size] [color]\tSets up the board size and your color for the new game.");
		System.out.println("auto\t\t\tSets up player one on this console as an automatic player.");
		System.out.println("manual\t\t\tSets up player one as a manual player on this console.");
		System.out.println("host [ID]\tSets up this program as a host with given ID for a remote game.");
		System.out.println("join [ID]@[hostname]\tJoins a remote game and sets this console to remote mode.");
		System.out.println("help\t\t\tThis message");
		System.out.println("dump\t\t\tDumps the board as a visual string representation.");
		System.out.println("load [file]\tSubstitutes the contents of [file] for console input until it is completely read.");
		System.out.println("seed [N]\tSets the random number generator to this seed value.");
		System.out.println("---------");
	}
	
	void dump() {
		System.out.println(board);
	}
	
	void load(String filename) {
		
		try {
			Scanner newIn = new Scanner(new File(filename));
			player[0].setAlternativeInput(newIn);
		} catch (FileNotFoundException e) {
			throw new IllegalArgumentException("The filename supplied could not be read from.");
		}
		
	}
	
	void seed(long seed) {
		if (getPlayer(1).type().equals("Machine")) ((MachinePlayer) getPlayer(1)).setSeed(seed);
		if (getPlayer(2).type().equals("Machine")) ((MachinePlayer) getPlayer(2)).setSeed(seed);
	}
	
	void clearRemote() {
			Color winner = getPlayer(1).getColor();
			setWinner(winner);
			displayWinner();
			clear();
	}
	
	/** Handles a quit command from the remote user. */
	void quitRemote() {
		isConnected = false;
		if (state != FINISHED) {
			state = FINISHED;
			player[1].close();
			player[1] = player[1].automatedPlayer();
			setExpectedCommand(null, 0);
			Color winner = getPlayer(1).getColor();
			setWinner(winner);
			displayWinner();
		} else {
			clear();
		}
	}
	
	/** Gets called when the remote player issues an illegal sequence of commands, and
	 * thus the remote game gets dropped. */
	void illegalRemoteOperation(String msg) {

		isConnected = false;
		player[1] = player[1].automatedPlayer();
		if (state != FINISHED) {
			setWinner(getPlayer(1).getColor());
			displayWinner();
			board = new Board(board.size());
			state = SETUP;
			throw new IllegalArgumentException("The remote connected performed an illegal operation\n Remote mode is being dropped.");
		} else {
			quitRemote();
		}
		
		
	}
	
	void displayWinner() {
		try {
			getPlayer(1).sendCommand("\n" + getWinner().toWinnerString() + " wins.\n");
		} catch (Exception e) {
			System.out.println("\n" + getWinner().toWinnerString() + " wins.\n");
		}
	}
	
	boolean isConnected() { 
		return isConnected; 
	}
	void setConnected(boolean val) { 
		isConnected = val; 
	}
	
	boolean hasExpectedCommand() {
		return expectedCommand != null;
	}
	
	boolean checkExpectedCommand(Command command) {
		if (command.type.equals(expectedCommand)) {
			expectedCommand = null;
			expectedPlayer = 0;
			return true;
		}
		if (inRemoteMode()) {
			illegalRemoteOperation("The command " + expectedCommand + " was not recieved, " + command + " was recieved.\n");
		}
		return false;
	}
	/** Sets the next command that is expected and must be recieved from the given player.
	 * @param command The string representation / name of the expected string.
	 * @param player The number of the player that this is expected from. */
	void setExpectedCommand(String command, int player) {
		expectedCommand = command;
		expectedPlayer = player;
	}
	
	private String expectedCommand = null;
	private int expectedPlayer = 0;
	
	private boolean isConnected = false;
	private Player[] player = new Player[2];
	private State state;
	private Board board;
	private Scanner stdIn;
	
	private Color whoseTurn;
	private Color winner = null;
	
	/** The delimiter to use between commands. */
	final String INPUTPATTERN = "\\s*(#.*)?(\\r)?\\n";
	
	
}

