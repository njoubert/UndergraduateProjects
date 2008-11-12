import java.util.Scanner;
import java.util.NoSuchElementException;

class TicTacToe {
	
	/** Read a move in the form of two integers, row (1..N) and column (1..N)
	 *  where N is the board size, from INP, and apply it to BOARD. */
	static void move (Scanner inp, char[][] board, char who) {
		while (true) {
			System.out.print ("Move [ROW COL]: ");
			try {
				int row = inp.nextInt ();
				int col = inp.nextInt ();
				
				if (row >= 1 && row <= board.length && col >= 1 && col <= board.length 
						&& board[row-1][col-1] == 0) {
					board[row-1][col-1] = who;
					return;
				}
			} catch (NoSuchElementException e) {
				if (! inp.hasNext ()) {
					System.err.println ("Input ends.");
					System.exit (1);
				}
			}
			inp.nextLine ();
			System.err.println ("Illegal move.");
		}
	}
	
	static void printBoard (char[][] board) {
		for (int i = 0; i < board.length; i += 1) {
			if (i > 0) {
				for (int j = 0; j < board.length; j += 1) {
					System.out.print ("----");
				}
				System.out.println ();
			}
			char[] row = board[i];
			for (int j = 0; j < row.length; j += 1) {
				if (j > 0)
					System.out.print ("|");
				if (board[i][j] == 0)
					System.out.print ("   ");
				else
					System.out.printf (" %c ", board[i][j]);
			}
			System.out.println ();
		}
	}
	
	/** True iff B is a won game. */
	static boolean over (char[][] B) {
		int N = B.length;
		int sum;
		for (int i = 0; i < N; i += 1) {
			sum = 0;
			for (int j = 0; j < N; j += 1)
				sum += B[i][j];
			if (sum == N * 'X' || sum == N * 'O')
				return true;
			sum = 0;
			for (int j = 0; j < N; j += 1)
				sum += B[j][i];
			if (sum == N * 'X' || sum == N * 'O')
				return true;
		}
		sum = 0;
		for (int i = 0; i < N; i += 1)
			sum += B[i][i];
		if (sum == N * 'X' || sum == N * 'O')
			return true;
		sum = 0;
		for (int i = 0; i < N; i += 1)
			sum += B[i][N-i-1];
		if (sum == N * 'X' || sum == N * 'O')
			return true;
		return false;
	}
	
	/** True iff B is a tie. */
	static boolean tied (char[][] B) {
		if (over (B))
			return false;
		for (char[] row : B)
			for (char c : row) 
				if (c == 0)
					return false;
		return true;
	}
	
	static public void main (String[] args) {
		Scanner inp = new Scanner (System.in);
		
		int N;
		if (args.length > 0)
			N = Integer.parseInt (args[0]);
		else
			N = 3;
		char[][] board = new char[N][N];
		int count = 0;
		while (true) {
			count += 1;
			if (count == 3) {
				System.out.println ("Tied!");
				break;
			}
			if (tied (board)) {
				System.out.println ("Tied!");
				break;
			}
			move (inp, board, 'X');
			printBoard (board);
			if (over (board)) {
				System.out.println ("You win!");
				break;
			}
			

			chooseMove (board, 'O');  /* That's a capital O, not zero */
			System.out.printf ("%nI move:%n");
			printBoard (board);
			if (over (board)) {
				System.out.println ("I win!");
				break;
			}
		}
	}
	
	static void chooseMove (char[][] B, char who) {
		for (char[] row : B) {
			for (int i = 0; i < row.length; i += 1)
				if (row[i] == 0) {
					row[i] = who;
					return;
				}
		}
	}

	
}

