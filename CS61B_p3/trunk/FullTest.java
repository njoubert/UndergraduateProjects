/* Author: *Niels Joubert CS61B-BO* */

import game.BoardTest;
import game.CommandTest;
import game.PlayersTest;
import junit.framework.TestCase;
import junit.framework.Test;
import junit.framework.TestSuite;

public class FullTest extends TestCase {

  public FullTest (String name) {
    super (name);
  }

  static public Test suite () {
    TestSuite suite = new TestSuite ();

    suite.addTestSuite (game.GameTest.class);
    suite.addTestSuite (BoardTest.class);
    suite.addTestSuite (PlayersTest.class);
    suite.addTestSuite (CommandTest.class);
    return suite;
  }
}
    
