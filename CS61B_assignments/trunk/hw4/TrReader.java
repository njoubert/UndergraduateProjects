import java.io.*;

/** Translating Reader: a stream that is a translation of an
 *  existing reader. */

public class TrReader extends Reader {
  /** A new TrReader that produces the stream of characters produced
   *  by STR, converting all characters that occur in FROM to the
   *  corresponding characters in TO.  That is, change occurrences of
   *  FROM.charAt(0) to TO.charAt(0), etc., leaving other characters
   *  unchanged.  FROM and TO must have the same length. */
	private Reader inpStream;
	private String fromString;
	private String toString;
	
  public TrReader (Reader str, String from, String to) { 
	  inpStream = str;
	  fromString = from;
	  toString = to;
  }

  // FILL IN
  // NOTE: Until you fill in the right methods, the compiler will
  //       reject this file, saying that you must declare TrReader
  // 	   abstract.  Don't do that; define the right methods instead!
  public int read() throws IOException {
	  char blah = (char) inpStream.read();
	  if ((int) blah != -1 && fromString.contains(blah + "")) {
		  int index = fromString.indexOf(blah);
		  return (int) toString.charAt(index);
	  } else {
		  return (int) blah;
	  }
  }
  
  
  public void close() {
	  try {
		  inpStream.close();
	  } catch (IOException e) {
		  
	  }
  }
  
  public int read(char[] cbuf, int off, int len) throws IOException {
      while (off < len) {
    	  int nextChar = read();
    	  if (nextChar < 0) {
    		  break;
    	  } else {
    		  cbuf[off] = (char) nextChar;
    		  off++;
    	  }
      }
      return 1;
  }
  
}


