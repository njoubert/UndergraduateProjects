/* Author: NIELS JOUBERT CS61B-BO */

import java.util.Scanner;
import java.util.regex.*;
import java.util.NoSuchElementException;
import java.util.Iterator;

/** A sequence of Assertions and Questions parsed from a given file. */
class PuzzleParser {
	
	private Scanner myInput;
	private Pattern myPattern = Pattern.compile("\\s*([^.?]+([.?]))\\s*");
	private boolean hasAssertion = false;
	private boolean hasQuestion = false;
	private boolean parseError = false;
	private String nextSentence;
	

  /** A PuzzleParser whose input comes from INPUT. */
  PuzzleParser (Scanner input) {
    myInput = input;
    readAndClassifyNextSentence();
  }
  
  PuzzleParser () { }

  /** True iff the next sentence in the input is an Assertion. */
  boolean hasAssertion () {
    return hasAssertion;
  }

  /** The next Assertion from the input.  Requires hasAssertion (). */
  Assertion nextAssertion () throws IllegalStateException {
    if (hasAssertion == false) {;
    	throw new IllegalStateException();
    }
	try {
	    Assertion newAssertion = Assertion.create(nextSentence);
	    readAndClassifyNextSentence();
	    return newAssertion;		  
	  } catch (IllegalArgumentException e) {
		  hasAssertion = false;
		  hasQuestion = false;
		  parseError = true;
		  return null;
	  }
  }

  /** True iff the next sentence in the input is an Question. */
  boolean hasQuestion () {
    return hasQuestion;
  }

  /** The next Question from the input.  Requires hasQuestion (). */
  Question nextQuestion () throws IllegalStateException{
	  if (hasQuestion() == false) {
	    	throw new IllegalStateException();
	    }
	  try {
		  Question newQuestion = Question.create(nextSentence);
		  readAndClassifyNextSentence();
		  return newQuestion;		  
	  } catch (IllegalArgumentException e) {
		  hasAssertion = false;
		  hasQuestion = false;
		  parseError = true;
		  return null;
	  }
	 
  }

  /** This returns the currently stored sentence */
  String currentSentence() {
	  return nextSentence;
  }
  /** True iff there is an erroneous statement or question next in 
   *  the input.  When true, hasAssertion and hasQuestion will be
   *  false. */
  boolean error () {
    return parseError;
  }

  /** 
   * This is the brains of this class - it checks to see whether we have input, 
   * tries to match the input against out known pattern, 
   * if not we know there's an error. If it matches,
   * we continue to classify it according to what matched, set the correct flags
   * and await the next call.
   */
  private void readAndClassifyNextSentence() {
	  if (myInput.hasNext() == true) {
		  try {
			  myInput.findWithinHorizon(myPattern, 0);
			  MatchResult result = myInput.match();
			  nextSentence = result.group(1);
			  if (result.group(2).equals(".") && hasQuestion == false) {
				  hasAssertion = true;
			  } else if (result.group(2).equals("?")) {
				  hasAssertion = false;
				  hasQuestion = true;
			  } else {
				  hasAssertion = false;
				  hasQuestion = false;
				  parseError = true;
			  }
		  } catch (NoSuchElementException e) {
			  hasAssertion = false;
			  hasQuestion = false;
			  parseError = true;
		  } catch (IllegalStateException e) {
			  hasAssertion = false;
			  hasQuestion = false;
			  parseError = true;
		  }
	  } else {
		  hasAssertion = false;
		  hasQuestion = false;
		  myInput.close();
	  }
  }

}

