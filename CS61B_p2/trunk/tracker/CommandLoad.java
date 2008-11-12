package tracker;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.Scanner;
import java.io.IOException;

/**
 * Handles the command "load"
 */
class CommandLoad extends Command {
	
	String parse (String commandText, Scanner inp, State theState) throws ProblemException, ParsedException {
		if (!commandText.equals("load")) return null;
		return execute(theState, inp.next());
	}
	
	private String execute(State theState, String filename) throws ProblemException, ParsedException {
		try {
			System.out.print("> ");
			FileInputStream fileHandle = new FileInputStream (filename);
			theState.getMyMain().driverLoop(new Scanner(fileHandle));
			fileHandle.close();
		} catch (FileNotFoundException e) {
			throw new ParsedException(filename, e.getMessage());
		} catch (IOException e) {
			throw new ParsedException(filename, e.getMessage());
		}
		return "load";
	}
	
}
