import java.io.*;  

public class Translate {
  /** The String S, but with all characters that occur in FROM changed
   *  to the corresponding characters in TO. FROM and TO must have the 
   *  same length. */
	
	static String translate (String S, String from, String to)
    {
        // NOTE: This try {...} catch is a technicality to keep Java happy.
        try { 
            Reader input = new StringReader(S);
            Reader output = new TrReader(input, from, to);
            char[] c = new char[S.length()];
            output.read(c, 0, S.length());
            return new String(c);
        } catch (IOException e) { return null; }
    }
	

  
  public static void main(String[] args) {
	  System.out.println(translate("yes manne hoe gaan dit daarso? cant you butt off?", "abc", "ABC"));
  }
  // REMINDER: translate must
  //  a. Be non-recursive
  //  b. Contain only 'new' operations, and ONE other method call, and no
  //     other kinds of statement (other than return).
  //  c. Use only the library classes String, and anything containing
  //     "Reader" in its name (browse the on-line documentation).
}
