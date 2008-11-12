package tracker;

/** An exception class that indicates some kind of error in input data or 
 *  commands. */
class ProblemException extends Exception {
  /** A Problem with no message. */
  ProblemException () { }

  /** A Problem with message MSG. */
  ProblemException (String msg) {
    super (msg);
  }
}
