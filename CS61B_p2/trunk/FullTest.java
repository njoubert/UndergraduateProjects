/* Author: Niels Joubert cs61b-bo */

import junit.framework.TestCase;
import junit.framework.Test;
import junit.framework.TestSuite;

public class FullTest extends TestCase {

  public FullTest (String name) {
    super (name);
  }

  static public Test suite () {
    TestSuite suite = new TestSuite ();

    suite.addTestSuite (tracker.TrackTest.class);
    suite.addTestSuite (util.UtilTest.class);
    suite.addTestSuite (tracker.StateTest.class);
    return suite;
  }
}