package game;

import java.util.Scanner;

/** A kind of Player that reads its commands from a GUI and displays
 *  the state of the board. */
class GUIPlayer extends Player {
	
	String type() {
		if (machine != null) {
			return machine.type();
		} else return "human";
	}
	
	GUIPlayer () {
		frame = new JumpingCubeFrame ("Niels' JumpingCube61B");
		//frame.setResizable(false);
		machine = null;
	}
	
	/** Returns the next command to be processed from this user.
	 * This can be from an elternative input if Load was used,
	 * from the GUI interface, or from
	 * a machine player. */
	String nextCommand () {
		frame.setBoard(getGame().getBoard());
		
		if (machine != null && getGame().getState() == Game.State.PLAYING)
			return machine.nextCommand();
		
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
		frame.enableMouseInput(true);
		String s = frame.getCommand();
		frame.enableMouseInput(false);
		return s;
	}
	
	/** Sends a command to the user, displaying it on the screen's status bar. */
	void sendCommand(String msg) {
		frame.setBoard(getGame().getBoard());
		if (!msg.matches("\\s*>\\s*"))
			frame.showStatusMessage(msg);
	}
	
	/** Informs the user of an error. */
	void error (String msg) {
		frame.showErrorDialog(msg);
	}
	
	void setGame (Game game) {
		super.setGame (game);
		frame.setBoard (game.getBoard ());
	}
	
	/** Set getColor () to COLOR, which must be RED or BLUE. */
	void setColor (Color color) {
		super.setColor(color);
		if (machine != null) machine.setColor(color);
	}
	
	Player automatedPlayer () { 
		machine = new MachinePlayer ();
		machine.setGame (getGame ());
		machine.setColor (getColor ());
		return this;
	}
	
	Player manualPlayer () {
		if (machine != null)
			machine.close ();
		machine = null;
		return this;
	}
	
	void close() {
		frame.dispose();
		Main.exit(0);
	}
	
	/** The GUI that I control. */
	private JumpingCubeFrame frame;
	/** A MachinePlayer to which I delegate the computation of moves when
	 *  player #1 is a machine. */
	private MachinePlayer machine;
	
}
