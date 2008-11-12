package tracker;

/** An exception class that indicates the program should exit.  Used as
 *  substitute for System.exit when running JUnit tests. */
class ExitException extends RuntimeException {

  final int code;

  /** An exit with completion code CODE. */
  ExitException (int code) {
    super ();
    this.code = code;
  }

}
