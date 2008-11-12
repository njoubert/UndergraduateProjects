package game;

import junit.framework.TestCase;
import static game.Color.*;

/*
 * This class tests the Board implementation.
 * Tests are as follows:
 * - Creating a blank board.
 * - Copying one board to a new board
 * - etc.
 */
public class BoardTest extends TestCase {

	
	/**
	 * All we are testing is creating a blank board
	 * that should by default be only white squares with one 
	 * spot in each.
	 * We also do a very unrigorous, quick
	 * test of all the methods concerned with the basic
	 * encoding of colors and values:
	 * 
	 * spots(r,c) and color(r,c)
	 * and thus getBoardValueAt(r,c)
	 *
	 */
	public void testBoardCreation() {
		Board someBoard = new Board(6);
		for (int r = 1; r <= someBoard.size(); r++) {
			for (int c = 1; c <= someBoard.size(); c++) {
				assertEquals(1, someBoard.spots(r, c));
				assertEquals(WHITE, someBoard.color(r, c));
			}
		}
	}
	
	public void testSmallBoard() {
		Board smallBoard = new Board(2);
		assertEquals(2, smallBoard.size());
	}
	
	public void testAddOne() {
		Board someBoard = new Board(6);
		someBoard.addSpot(RED, 1, 1);
		assertEquals(2, someBoard.spots(1, 1));
		assertEquals(RED, someBoard.color(1, 1));
		
		someBoard.addSpot(BLUE, 6, 6);
		assertEquals(2, someBoard.spots(6, 6));
		assertEquals(BLUE, someBoard.color(6, 6));
		
	}
	
	public void testSpotConsistancy() {
		Board b = new Board(3);
		int totalSpots = 9;
		assertEquals(countSpots(b), totalSpots);
		
		//Add to center
		b.addSpot(RED, 2, 2);
		totalSpots++;
		assertEquals(countSpots(b), totalSpots);
		//Add to center and it spills
		b.addSpot(RED, 2, 2);
		totalSpots++;
		b.addSpot(RED, 2, 2);
		totalSpots++;
		assertEquals(countSpots(b), totalSpots);
		//Add to border for spill
		b.addSpot(RED, 2, 3);
		totalSpots++;
		b.addSpot(RED, 2, 3);
		totalSpots++;
		assertEquals(countSpots(b), totalSpots);
		//Add to corner for spill
		b.addSpot(RED, 1, 1);
		totalSpots++;
		assertTrue(!b.isFull());
	}
	
	public void testInnerOneJump() {
		Board b = getInterleavedBoard(3);
		String startBString = "===\n    2r 2b 2r \n    2r 2b 2r \n    2r 2b 2r \n===";
		
		String addTwoBString = "===\n    2r 2b 2r \n    2r 4b 2r \n    2r 2b 2r \n===";
		
		String jumpedBBString = "===\n    2r 3b 2r \n    3b 1b 3b \n    2r 3b 2r \n===";
		
		assertTrue(startBString.equals(b.toDisplayString()));
		
		b.addSpot(BLUE, 2, 2);
		b.addSpot(BLUE, 2, 2);
		assertTrue(addTwoBString.equals(b.toDisplayString()));
		
		b.addSpot(BLUE, 2, 2);
		assertTrue(jumpedBBString.equals(b.toDisplayString()));
	}

	
	public void testIsLegalFullSquare() {
		Board fullRedBoard = new Board(3);
		int totalSpots = 0;
		for (int r = 1; r <= fullRedBoard.size(); r++) {
			for (int c = 1; c <= fullRedBoard.size(); c++) {
				fullRedBoard.addSpot(RED, r, c);
				totalSpots += 2;
			}
		}
		assertTrue(!fullRedBoard.isLegal(RED));
	}
	
	public void testBorderCase1() {
		Board blah = new Board(6);
		for (int i = 0; i < 20; i++) {
			blah.addSpot(RED, 1, 1);
		}
		//System.out.println(blah);
		assertEquals(56, countSpots(blah));
		
	}
	
	public void testIsLegalLooper() {
		Board blah = new Board(6);
		int r = 1, c = 1;
		while (blah.isLegal(RED)) {
			if (c > blah.size()) {r += 1; c = 1;}
			blah.addSpot(RED, r, c);
			c++;
		}
		assertEquals(RED, blah.getWinner(RED));
	}
	
	public void testToDisplayString() {
		String redBString = "===\n    2r 2r 2r \n    2r 2r 2r \n    2r 2r 2r \n===";
		String interleavedBString = "===\n    2r 2b 2r \n    2r 2b 2r \n    2r 2b 2r \n===";
		String emptyBString = "===\n    -- -- -- \n    -- -- -- \n    -- -- -- \n===";
		assertTrue(redBString.equals(getRedBoard(3).toDisplayString()));
		assertTrue(interleavedBString.equals(getInterleavedBoard(3).toDisplayString()));
		assertTrue(emptyBString.equals(getEmptyBoard(3).toDisplayString()));

		
	}
	
	public void testSecondConstructor() {
		Board first = getInterleavedBoard(6);
		first.addSpot(RED, 3, 3);
		assertEquals(73, countSpots(first));
		
		Board second = new Board(first);
		
		assertEquals(73, countSpots(second));
		assertEquals(RED, second.color(3, 3));
		

		//Lets make sure the two boards are disconnected.
		second.addSpot(RED, 5, 5);
		assertEquals(74, countSpots(second));
		assertEquals(73, countSpots(first));

	}
	public void testErrors() {
		Board me = getEmptyBoard(6);
		
		//Outside board?
		try {
			me.addSpot(RED, 10, 10);
			assertTrue(false);
		} catch (IllegalArgumentException e) {
			assertTrue(true);
		}
		
		//Now my color?
		me.addSpot(RED, 1, 1);
		try {
			me.addSpot(BLUE, 1, 1);
			assertTrue(false);
		} catch (IllegalArgumentException e) {
			assertTrue(true);
		}
	}
	
	public void testSomeError1() {
		Board b = new Board(3);
		b.addSpot(BLUE, 1, 1);
		b.addSpot(BLUE, 2, 1);
		b.addSpot(BLUE, 2, 1);
		b.addSpot(BLUE, 1, 2);
		b.addSpot(BLUE, 1, 2);
		b.addSpot(RED, 2, 2);
		b.addSpot(RED, 2, 3);
		b.addSpot(RED, 3, 2);
		b.addSpot(RED, 3, 2);
		b.addSpot(RED, 3, 3);
		assertEquals("===\n    2b 3b -- \n    3b 2r 2r \n    -- 3r 2r \n===", b.toString());
		b.addSpot(BLUE, 2, 1);
		assertEquals("===\n    2b 1b 2b \n    2b 4b 2r \n    2b 3r 2r \n===", b.toString());
	}
	
	//This is based on a game played in KJumpingCube
	public void testActualGame() {
		Board brd = new Board(5);
		String brdStr = "";
		
		brd.addSpot(RED, 1, 1);
		brd.addSpot(BLUE, 3, 1);
		brdStr = "===\n    2r -- -- -- -- \n    -- -- -- -- -- \n    2b -- -- -- -- \n    -- -- -- -- -- \n    -- -- -- -- -- \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 1, 3);
		brd.addSpot(BLUE, 3, 1);
		brdStr = "===\n    2r -- 2r -- -- \n    -- -- -- -- -- \n    3b -- -- -- -- \n    -- -- -- -- -- \n    -- -- -- -- -- \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 3, 5);
		brd.addSpot(BLUE, 3, 1);
		brdStr = "===\n    2r -- 2r -- -- \n    2b -- -- -- -- \n    1b 2b -- -- 2r \n    2b -- -- -- -- \n    -- -- -- -- -- \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 5, 1);
		brd.addSpot(BLUE, 3, 1);
		brdStr = "===\n    2r -- 2r -- -- \n    2b -- -- -- -- \n    2b 2b -- -- 2r \n    2b -- -- -- -- \n    2r -- -- -- -- \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 5, 1);
		brd.addSpot(BLUE, 3, 2);
		brdStr = "===\n    2r -- 2r -- -- \n    2b -- -- -- -- \n    2b 3b -- -- 2r \n    3r -- -- -- -- \n    1r 2r -- -- -- \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 4, 1);
		brd.addSpot(BLUE, 5, 5);
		brdStr = "===\n    2r -- 2r -- -- \n    2b -- -- -- -- \n    3r 3b -- -- 2r \n    1r 2r -- -- -- \n    2r 2r -- -- 2b \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 3, 1);
		brd.addSpot(BLUE, 5, 5);
		brdStr = "===\n    2r -- 2r -- -- \n    3r -- -- -- -- \n    1r 4r -- -- 2r \n    2r 2r -- -- 2b \n    2r 2r -- 2b 1b \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 2, 1);
		brd.addSpot(BLUE, 4, 5);
		brdStr = "===\n    1r 2r 2r -- -- \n    2r 2r -- -- -- \n    2r 4r -- -- 2r \n    2r 2r -- -- 3b \n    2r 2r -- 2b 1b \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 1, 2);
		brd.addSpot(BLUE, 4, 5);
		brdStr = "===\n    1r 3r 2r -- -- \n    2r 2r -- -- -- \n    2r 4r -- -- 3b \n    2r 2r -- 2b 1b \n    2r 2r -- 2b 2b \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 1, 3);
		brd.addSpot(BLUE, 4, 4);
		brdStr = "===\n    1r 3r 3r -- -- \n    2r 2r -- -- -- \n    2r 4r -- -- 3b \n    2r 2r -- 3b 1b \n    2r 2r -- 2b 2b \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 1, 2);
		brd.addSpot(BLUE, 5, 4);
		brdStr = "===\n    2r 2r 1r 2r -- \n    2r 3r 2r -- -- \n    2r 4r -- -- 3b \n    2r 2r -- 3b 1b \n    2r 2r -- 3b 2b \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 2, 3);
		brd.addSpot(BLUE, 4, 4);
		brdStr = "===\n    2r 2r 1r 2r -- \n    2r 3r 3r -- -- \n    2r 4r -- -- 3b \n    2r 2r -- 4b 1b \n    2r 2r -- 3b 2b \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 1, 3);
		brd.addSpot(BLUE, 4, 4);
		brdStr = "===\n    2r 2r 2r 2r -- \n    2r 3r 3r -- -- \n    2r 4r -- 2b 3b \n    2r 2r 2b 2b 3b \n    2r 2r 2b 2b 1b \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 1, 2);
		brd.addSpot(BLUE, 3, 5);
		brdStr = "===\n    2r 3r 2r 2r -- \n    2r 3r 3r -- 2b \n    2r 4r -- 3b 2b \n    2r 2r 2b 3b 1b \n    2r 2r 2b 2b 2b \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 1, 2);
		brd.addSpot(BLUE, 2, 5);
		brdStr = "===\n    1r 2r 3r 2r -- \n    3r 4r 3r -- 3b \n    2r 4r -- 3b 2b \n    2r 2r 2b 3b 1b \n    2r 2r 2b 2b 2b \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 1, 2);
		brd.addSpot(BLUE, 2, 5);
		brdStr = "===\n    1r 3r 3r 2r 2b \n    3r 4r 3r 2b 1b \n    2r 4r -- 3b 3b \n    2r 2r 2b 3b 1b \n    2r 2r 2b 2b 2b \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		//BIG SHIFT COMING UP!
		brd.addSpot(RED, 1, 2);
		brd.addSpot(BLUE, 1, 5);
		brdStr = "===\n    2b 1b 2b 2b 2b \n    2r 3b 3b 4b 2b \n    2r 3r 3r 3b 3b \n    3r 3r 2b 3b 1b \n    2r 2r 2b 2b 2b \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 3, 2);
		brd.addSpot(BLUE, 1, 4);
		brdStr = "===\n    2b 1b 2b 3b 2b \n    2r 3b 3b 4b 2b \n    2r 4r 3r 3b 3b \n    3r 3r 2b 3b 1b \n    2r 2r 2b 2b 2b \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 3, 2);
		brd.addSpot(BLUE, 1, 3);
		brdStr = "===\n    2b 1b 3b 3b 2b \n    2r 4r 3b 4b 2b \n    3r 1r 4r 3b 3b \n    3r 4r 2b 3b 1b \n    2r 2r 2b 2b 2b \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 3, 1);
		brd.addSpot(BLUE, 1, 1);
		brdStr = "===\n    2b 3b 3b 3b 2b \n    2b 1b 4b 4b 2b \n    3b 4b 4r 3b 3b \n    3r 2r 3r 3b 1b \n    2r 1r 3r 2b 2b \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		brd.addSpot(RED, 4, 1);
		brd.addSpot(BLUE, 5, 5);
		brdStr = "===\n    2r 3r 3r 3r 2r \n    2r 1r 4r 4r 2r \n    3r 3r 3r 2r 2r \n    3r 4r 4r 4r 3b \n    1r 2r 3r 3b 1b \n===";
		assertEquals(brdStr, brd.toDisplayString());
		
		assertTrue(brd.isLegal(RED));
		brd.addSpot(RED, 4, 4);
		assertTrue(!brd.isLegal(RED));
	}
	
	static Board getRedBoard(int size) {
		Board board = new Board(size);
		for (int r = 1; r <= board.size(); r++) {
			for (int c = 1; c <= board.size(); c++) {
				board.addSpot(RED, r, c);
			}
		}
		return board;
	}
	
	static Board getInterleavedBoard(int size) {
		Board board = new Board(size);
		for (int r = 1; r <= board.size(); r++) {
			board.addSpot(RED, r, 1);
		}
		for (int r = 1; r <= board.size(); r++) {
			for (int c = 2; c <= board.size(); c++) {
				board.addSpot(board.color(r,c-1).opposite(), r, c);
			}
		}
		return board;
	}
	static Board getEmptyBoard(int size) {
		return new Board(size);
	}
	
	/** Helpful for checking that spot number does not change over time
	 * as hopping takes place.
	 */
	static int countSpots(Board b) {
		int totalSpots = 0;
		for (int r = 1; r <= b.size(); r++) {
			for (int c = 1; c <= b.size(); c++) {
				totalSpots += b.spots(r, c);
			}
		}
		return totalSpots;
	}
	
}


