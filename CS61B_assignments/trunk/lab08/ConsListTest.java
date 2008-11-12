import junit.framework.TestCase;

public class ConsListTest extends TestCase {

  public ConsListTest (String title) {
    super (title);
  }

  private ConsList make (String... args) {
    ConsList L = new ConsList ();
    for (String x : args)
      L.add (x);
    return L;
  }

  public void testAdd () {
    ConsList L = new ConsList ();
    L.add ("10");
    L.add ("15");
    L.add (0, "3");
    L.add (0, "2");
    L.add (4, "20");
    L.add (2, "8");
    assertEquals ("[2, 3, 8, 10, 15, 20]", L.toString ());
  }

  public void testRemove () {
    ConsList L1 = make ("1", "2", "3", "4", "5", "6");
    L1.remove (0);
    L1.remove (4);
    L1.remove (1);
    assertEquals ("[2, 4, 5]", L1.toString ());

    ConsList L2 = make ("1", "2", "3", "4", "5", "6");
    L2.remove ("1");
    L2.remove ("6");
    L2.remove ("3");
    assertEquals ("[2, 4, 5]", L2.toString ());

    assertTrue (L1.equals (L2));

  }



}

