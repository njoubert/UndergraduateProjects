package game;

import java.io.IOException;

import junit.framework.TestCase;
import java.util.ArrayList;

/**
 * We test the Command implementation.
 * We create a mock player and game class
 * and we instantiate this fake game with the fake player.
 * We now use our Command class to create command
 * objects, each time passing in a mock string
 * and checking that the object we get back
 * corresponds to the actual object created.
 * 
 */
public class CommandTest extends TestCase {

	
	public void testCommand() {
		String commandS;
		Command expectedC;
		Player player = new FakePlayer();
		Player otherPlayer = new FakePlayer();
		Color color = Color.RED;
		FakeGame game = new FakeGame(player);
		Command retC;
		
		for (int i = 0; i < Command.allCommandStrings.length; i+= 1) {
			commandS = Command.allCommandStrings[i];
			expectedC = Command.allCommandTypes[i];
			retC = Command.create(commandS, (Game) game, player, otherPlayer, color);
			assertEquals(expectedC.getClass().getName(), retC.getClass().getName());
			
			try {
				retC.execute();
				retC.report();
				
			} catch (ExitException e) {
				
			} catch (Exception e) {
				assertTrue(false);
			}
			
		}
		
		
	}
	
}

class FakeGame extends Game {
	
	FakeGame(Player player) {
		super(player);
		super.game(6, Color.RED);
	}
	
	/*
	 * METHODS FOR EXECUTING COMMANDS FROM PARSERS
	 * 
	 */
	
	void start() {
	}
	
	void quitLocal() {
	}
	
	void quitRemote() {	}
	
	void game(int size, Color color) {	}
	
	void auto() {
	}
	
	void manual() {
	}
	
	void host(String id) throws IOException {	}
	
	void join(String id, String host) throws IOException {	}
	
	void move(int r, int c, Color color) {	}
	
	void help() {}
	
	void dump() {	}
	
	void load(String filename) {	}
	
	void seed(long seed) {	}
	
}

class FakePlayer extends Player {
	
	ArrayList<String> commands = new ArrayList<String>();
	ArrayList<String> reports = new ArrayList<String>();
	String type = "Fake";
	
	String type() {
		return type;
	}
	
	void setType(String type) {
		this.type = type;
	}
	
	void putNextCommand(String command) {
		commands.add(command);
	}
	
	void sendCommand(String msg) {
		reports.add(msg);
	}
	
	String getNextReport() {
		return reports.remove(0);
	}
	
	Player automatedPlayer () {
		return this;
	}
	
	/** Returns a manual version of myself. */
	Player manualPlayer () {
		return this;
	}
	
	/** The next command or move from THIS.  Uses the format of commands
	 *  documented in the specification.  A return value of null indicates
	 *  that THIS has nothing to do at this point (for example, when its
	 *  game is in set-up mode, an automated player never issues commands). */
	String nextCommand () throws IOException {
		return commands.remove(0);
	}
		
	/** Inform THIS that the last command returned was erroneous.  MSG is
	 *  a description of the error. */
	void error (String msg) {
		throw new IllegalArgumentException(msg);
	}
}
