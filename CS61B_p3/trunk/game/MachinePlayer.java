package game;

import java.util.Random;
import java.util.Scanner;
import java.util.ArrayList;

/** A kind of Player that computes commands. 
 * 
 * I am implementing a gametree search structure with a heuristic position findin procedure to 
 * create an A.I.
 * 
 * The general algorithm follows that of the Alpha-Beta pruning of a MiniMax tree algorithm.
 * 
 * A) Start with an initial board, and identify the initial candidate moves.
 * 
 * B) For each of these candidate moves, find a value with which it can be compared
 * with to the other candidate positions. This is the minimax algorithm.
 * 
 *  C) To find the value of a move, do the move, find all the
 *  good moves after this move has been done, make a decision (alpha-beta pruning)
 *  whether you want to search down this path, and again search for the values of this
 *  move. 
 *  
 *  D) Once you reach the bottom of your tree, yu start assigning values to
 *  boards, so that you build up a stack of values, at each level battling
 *  it out with previous values, until the values were filtered to the top and 
 *  by simple comparison you can identify the best move. 
 *  
 *  This was the plan at least, but after several attempts, I abandoned this for
 *  a random move generator.*/

class MachinePlayer extends Player {
	
	private Random generator;
	
	MachinePlayer() {
		generator = new Random();
	}
	
	String type() {
		return "Machine";
	}
	
	void setSeed(long seed) {
		generator = new Random(seed);
	}

	/**
	 * Either reads input from the terminal in the same manner as the HumanPlayer
	 * if we are in setup or finished state, or asks the A.I. to generate
	 * a move if we are in playing state.
	 */
	String nextCommand () {
		if (getGame().getState() == Game.State.PLAYING) {
			int[] move = calculateRandomMove();
			return move[0] + ":" + move[1];
		} else if (alternativeInput != null) {
			
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
	
	void error (String msg) {
		System.out.println(msg);
	}
	
	Player automatedPlayer () {
		return this;
	}
	
	Player manualPlayer () {
		Player newMe = new HumanPlayer ();
		newMe.alternativeInput = this.alternativeInput;
		newMe.setGame (getGame ());
		newMe.setColor (getColor ());
		return newMe;
	}
	
	
	
								/*
								 * =====================
								 *   MOVING STRUCTURE
								 * =====================
								 */
	
	private Board initial;
	private Board temp;
	
	/**
	 * Generates the move to be played on the board.
	 * <br>This is a very simple procedure of finding a random square that
	 * is legal for this player.
	 */
	int[] calculateRandomMove() {
		int r = generator.nextInt(getGame().size()) + 1;
		int c = generator.nextInt(getGame().size()) + 1;
		while(!getGame().isLegal(getColor(), r, c)) {
			
			r = generator.nextInt(getGame().size()) + 1;
			c = generator.nextInt(getGame().size()) + 1;
			
			if (!getGame().isLegal(getColor())) break;
			if (r < 1) r += 1;
			if (r > getGame().size()) r = 1;
			if (c < 1) c+= 1;
			if (c > getGame().size()) c = 1;
			
		} 
		return new int[]{r, c};
	}
	
	/**
	 * Generates the move to be played on the board.
	 * This is an algorithm that checks one move ahead to find the optimal
	 * move in terms of adding to your own spots and spreading your color.
	 * @return
	 */
	int[] calculateMove() {
		initial = getGame().getBoard();
		return findBestMove(generateInitialMoves(getColor()), initial, getColor());
	}
	
	/*
	 * Generate the Initial Moves
	 */
	ArrayList<int[]> generateInitialMoves(Color player) {
		ArrayList<int[]> moves = new ArrayList<int[]>();
		
		//Lets pick out all the spots that belongs to us
		for (int r = 1; r <= initial.size(); r++) {
			for (int c = 1; c <= initial.size(); c++) {
				if (initial.color(r, c) != player.opposite())
					moves.add(new int[]{r, c});
			}
		}
		return moves;
	}
	
	
	/*
	 * Find the best move of these initial moves
	 */
	int[] findBestMove(ArrayList<int[]> moves, Board current, Color player) {
		int[] bestMove = {1, 1};
		double val = Double.NEGATIVE_INFINITY;
		double moveVal;
		while (!moves.isEmpty()) {
			int[] move = moves.remove(generator.nextInt(moves.size()));
			moveVal = evaluate(move, player, current);
			if (moveVal > val) {
				val = moveVal;
				bestMove = move;
			}
		}
		return bestMove;
	}
	
	
	/*
	 * Search for the value of a specific move.
	 */
	
	
	/*
	 * Generate good moves for an arbitrary board (somewhere in the middle of the evaluation
	 */
	
	
	/*
	 * Evaluate a certain board.
	 */
	
	double evaluate(int[] move, Color player, Board current) {
		temp = new Board(current);
		double value = 1;
		
		if (current.color(move[0], move[1]) == player) 	//Weigh squares that are our color.
			value = value * 2;
		
		value = value * (current.spots(move[0], move[1]) / 2);	//Weight squares with more spots
		
		current.addSpot(player, move[0], move[1]);	//Check the spreading.
		value = value * (temp.hasSquares(player) - current.hasSquares(player) + 1) * 4; //And weigh it
		
		return value;
		
		
	}
}
