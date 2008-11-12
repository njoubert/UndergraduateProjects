package game;

/** An exception that indicates that the program should terminate. */
@SuppressWarnings("serial") class ExitException extends RuntimeException {
	
	ExitException () {
	}
	
	ExitException (String msg) {
		super (msg);
	}
	
}
