package game;

import static game.Color.*;

import java.io.IOException;
import java.util.Scanner;
import java.util.ArrayList;

/** <B>Represents one player in a game of Jump61b.</b>
 * <br>At any given time, each 
 *  Player is attached to a Game and has a Color.  Each call of the 
 *  nextCommand() returns a move or other command from the Player.  The 
 *  caller (a method within the Player's Game, usually) can either execute
 *  this command or call error to inform THIS that the command was wrong. 
 *  
 *  When actual play starts, Player #1 may be replaced by an automated 
 *  that takes over moving until the game ends.  When the game ends, 
 *  Player #1 goes back to being a manual player.  The methods
 *  automatedPlayer and manualPlayer return these versions of the player.
 *  We do it this way because a manual player that uses a GUI might differ
 *  from one that uses the terminal, and the corresponding automated players
 *  might differ as well.  This design allows us to avoid having to do 
 *  tests every time we switch between automated and manual modes to see
 *  which version of Player (GUI- or terminal-based) we need to switch to. */
abstract class Player {
	
	/** A Player, initially not attached to any Game and playing RED. */
	Player () {
		this.color = RED;
	}
	
	/** Returns the name of the type of this Player. */
	abstract String type();
	
	/** The color I am playing. */
	Color getColor () {
		return color;
	}
	
	/** Set getColor () to COLOR, which must be RED or BLUE. */
	void setColor (Color color) {
		assert color == RED || color == BLUE;
		this.color = color;
	}
	
	/** The Game I am currently playing in. */
	Game getGame () {
		return game;
	}
	
	/** Set the Game I am currently playing in to GAME. */
	void setGame (Game game) {
		this.game = game;
	}
	
	/** Perform any necessary clean-up operations.  This default 
	 *  implementation does nothing. */
	void close () {
	}
	
	/** Returns an automated version of myself. */
	abstract Player automatedPlayer ();
	
	/** Returns a manual version of myself. */
	abstract Player manualPlayer ();
	
	/** The next command or move from THIS.  Uses the format of commands
	 *  documented in the specification.  A return value of null indicates
	 *  that THIS has nothing to do at this point (for example, when its
	 *  game is in set-up mode, an automated player never issues commands). */
	abstract String nextCommand () throws IOException;
	
	/** The command that the other player entered, to send to this player.
	 * By default does nothing, seeing that not all players
	 * needs to recieve any messages.
	 * @param msg The command the other player just entered.
	 */
	abstract void sendCommand (String msg) throws IOException;
	
	/** Inform THIS that the last command returned was erroneous.  MSG is
	 *  a description of the error. */
	abstract void error (String msg);
	
	/** Sets an alternative input for this player.
	 * Sets the given scanner to use the pattern defined inside Game (INPUTPATTERN)
	 * as the delimited for this. */
	void setAlternativeInput(Scanner in) {
		if (alternativeInput == null) {
			alternativeInput = new ArrayList<Scanner>();
		}
		in.useDelimiter(getGame().INPUTPATTERN);
		alternativeInput.add(0, in);
		
	}
	
	void setNormalInput() {
		alternativeInput = null;
	}
	
	void flush(long millis) { }
	
	ArrayList<Scanner> alternativeInput = null;
	
	private Color color;
	private Game game;
	
}
