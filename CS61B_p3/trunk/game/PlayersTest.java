package game;

import junit.framework.TestCase;
import static game.Color.*;

/** This JUnit Test Class checks the remote players.
 * @author Niels Joubert CS61B-BO
 *
 */
public class PlayersTest extends TestCase {
	
	/** Connect to yourself as if you are a different computer
	 * and run all the stuff.
	 */
	public void testRemoteSendingCommands() {
		try {
			Player host = new HostPlayer("ID1");
			Player client = new ClientPlayer("localhost", "ID1");
			host.sendCommand("command1");
			assertEquals("command1", client.nextCommand());
			client.sendCommand("command2");
			assertEquals("command2", host.nextCommand());
			client.close();
			host.close();
		} catch (java.io.IOException e) {
			assertTrue(false);
		}
	}
	
	/** We simply test whether the AI manages to win the board. */
	public void testAI() {
		Player ai = new MachinePlayer();
		Board b = new Board(3);
		Game g = new Game(ai);
		g.start();
		g.setBoard(b);
		
		String command;
		
		int placedSpots = 9;
		int i = 9;
		for (; i < 30; i++) {
			try {
				command = ai.nextCommand();
				b = g.getBoard();
				if (b.isLegal(ai.getColor())) {
					b.addSpot(RED, Integer.parseInt(command.split(":")[0]), Integer.parseInt(command.split(":")[1]));
					g.setBoard(b);
					placedSpots += 1;
				} else break;
			} catch (java.io.IOException e) {
				assertTrue(false);
			}
		}
		assertEquals(i, placedSpots);
		b = g.getBoard();
		assertEquals(placedSpots, BoardTest.countSpots(b));
	}
	
	/** A rigorous test to make sure the human running the program actually knows what he's doing.
	 * Hacks his grade to a C- in CS61B if he fails the test.*/
	public void testHuman() {
		//Just Kidding!
/*		Player p = new HumanPlayer();
		while (true) {
			try {
			System.err.println(p.nextCommand());
			} catch (Exception e) {
				break;
			}
		}*/
	}
}
