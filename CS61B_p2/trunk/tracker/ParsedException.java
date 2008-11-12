package tracker;

/**
 * An exception class specifically used to 
 * signal an error in the arguments of a command.
 * It carries the argument that caused the exception
 * as a String, and is used ONLY to recover
 * gracefully if the illegal argument was extracted.
 * @author Administrator
 *
 */
class ParsedException extends Exception {
  /** Parsing exception with this command */
	ParsedException (String illegalToken) {
		this.illegalToken = illegalToken;
	}

  /** A Problem with message MSG. */
  ParsedException (String illegalToken, String msg) {
    super (msg);
    this.illegalToken = illegalToken;
  }
  
  String getIllegalToken() {
	  return illegalToken;
  }
  
  String illegalToken;
  
}