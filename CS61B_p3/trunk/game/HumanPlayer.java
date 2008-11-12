package game;

import java.util.Scanner;

/** A kind of Player that reads its commands from the standard input. 
 * <BR><h3>Parsing Input</h3>
 * <BR>Seeing that this is the player that reads from the human - that is, the standard input
 * of the system - we need to handle this. To prevent the scanner reading the terminal
 * ever to be accidentally closed becuase the HumanPlayer is garbage-collected, this scanner
 * is stored inside the Game class, which exists as long as the game is being played. 
 * We simply access this throught the getter messages supplied by the Game.
 * 
 * Upon the creation of this object, we create a Scanner for
 * the standard input, which is stored in a field. Whenever nextCommand is called, the next
 * line is read and normalized by removing comments and abundant whitespace so that it 
 * conforms to the stricter boundaries of the parsing system of the Game.
 * <BR><h4>Retrieving commands:</h4>
 * <P>During normal operation, the command returned by this player comes from
 * the standard input, therefore the Game's reproduction of the Scanner responsible
 * for the standard input it used to retrieve input if there is no alternative source specified.
 * <br>Since files can also be used as input, the
 * <BR> 
 * */
class HumanPlayer extends Player {
	
	String type() { 
		return "Human"; 
	}
	
	/**
	 * During normal play, reads in commands from the current System.in input and parses them into command strings
	 * that correspond to the expected commands. It removes comments and leading/trailing whitespace.
	 * If an alternative input scanner (holding a file) is specified, that is read until it is empty.
	 */
	String nextCommand () {
		if (alternativeInput != null) {
			
			if (!alternativeInput.isEmpty()) {
				Scanner in = alternativeInput.get(0);
				if (in.hasNext()) {
					return in.next();
				} else {
					alternativeInput.remove(0);
					return nextCommand();
				}
			} else {
				alternativeInput = null;
			}
			
		}
		return getGame().stdInNext().trim();
	}
	
	void sendCommand(String msg) {
		System.out.print(msg);
	}
	/** Prints the error message to the standard error output.
	 * This gives the appropriate error message to the user.
	 */
	void error (String msg) {
		System.out.println(msg);
	}
	
	Player automatedPlayer () {
		Player newMe = new MachinePlayer ();
		newMe.alternativeInput = this.alternativeInput;
		newMe.setGame (getGame ());
		newMe.setColor (getColor ());
		return newMe;
	}
	
	Player manualPlayer () {
		return this;
	}
	
}
