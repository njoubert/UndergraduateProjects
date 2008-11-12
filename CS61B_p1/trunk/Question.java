import java.util.List;
import java.util.LinkedList;
import java.util.Iterator;

/* Author: NIELS JOUBERT CS61B-BO */

/** Represents a puzzle question. */
abstract class Question {
  
	protected String storedQuestion;
	protected String myName = null;
	protected String myOccupation = null;
	protected String myColor = null;
	
	/** an array of all the known question types.
	 * this is used by the create() method to create a question subtype.
	 */
	static private final Question[] questionTypes = {
		  new QWhatDoYouKnowAboutName(),
		  new QWhatDoYouKnowAboutTheOccupation(),
		  new QWhatDoYouKnowAboutTheColorHouse(),
	      new QWhoIsTheOccupation (),
	      new QWhoLivesInTheColorHouse (),
	      new QWhatDoesTheOccupantOfTheColorHouseDo(),
	      new QWhatDoesNameDo (),
	      new QWhereDoesNameLive(),
	      new QWhereDoesTheOccupationLive()
	   };
	
	
  /** A printable version of THIS.  Returns a sentence in the syntax
   *  defined by the puzzle problem statement, such as "Who is the
   *  carpenter?" */
  public String toString () {
    return storedQuestion;
  }

  /** The name, if any, mentioned in THIS; otherwise null.  */
  String getName () {
    return myName;
  }

  /** The occupation, if any, mentioned in THIS; otherwise null.  */
  String getOccupation () {
    return myOccupation;
  }

  /** The house color, if any, mentioned in THIS; otherwise null.  */
  String getColor () {
    return myColor;
  }
  
  /** Takes in an answer set - a 2D array of variable length consisting of triples.
   * Each triple consists of {name, occupation, color}. 
   * Find the triple that has the given value in the given field.
   * Fields are:
   * 0 = name
   * 1 = occupation
   * 2 = color
   */
  public static String[] extractTriple(int field, String value, String[][] answerS) throws ArrayIndexOutOfBoundsException, IllegalArgumentException {
	  
	  if (field < 0 || field > 3) throw new IllegalArgumentException();
	  
	  for (int i = 0; i < answerS.length; ++i) {	//Runs through all the triples in the answerSet
		  
		  if (answerS[i][field].equals(value)) {
			  return answerS[i].clone();			//If we find it, return it.
		  }
		  
	  }
	  
	  throw new ArrayIndexOutOfBoundsException();	//If we didnt find it, something went horribly wrong.
  }
  
  /** This is the answer generator.
   * It takes a set of possibilities allowed by the assertions.
   * It finds the requested triple in each possibility set (represented as an array of triples)
   * and compiles this into the best possible info that can be extracted
   * from the triples that satisfy the requirements in the possible set.
   * 
   * @param field - an integer: 0 == name, 1 == occupation, 2 == color - the field to extract.
   * @param value - a String - the value you want the specified field to be.
   * @param possibleS - a LinkedList of possibilities, each represented by a 2D string of triples.
   * @return the tripe that has the most information we could find.
   */
  public static String[] findAnswer(int field, String value, LinkedList<String[][]> possibleS) {
	  String[] answer = new String[3];
	  
	  Iterator<String[][]> possibilitiesIterator = possibleS.iterator();
	  String[][] currPossibility;
	  //Go through each set of possibilities:
	  while (possibilitiesIterator.hasNext()) {
		  
		  currPossibility = possibilitiesIterator.next();
		  //Get the triple corresponding to the field and value we want
		  String[] thisTriple = extractTriple(field, value, currPossibility);
		  //Check this against what we know so far:
		  
		  for (int i = 0; i < thisTriple.length; i++) {
			  if (answer[i] == null) {
				  answer[i] = thisTriple[i];		//Ooh, we found some info! save it!
			  } else if (!answer[i].equals(thisTriple[i])) {
				  answer[i] = "#u";					//Sorry, this contraicts what we thought we knew, set unknown!
			  } else {
				  // answer[i] = thisTriple[i];	  They are the same, so we do not need to set it again.
			  }
			  
		  }
		  
	  }
	  
	  return answer.clone();
  }
  
  	/** Returns a specific class if the given argument is valid */
  public abstract Question parse(String unparsedQuestionString);


  	/** Creates a specific Question class of a subtype of question */
  public static Question create(String unparsedQuestionString) throws IllegalArgumentException {
      
	  for (Question questionType : questionTypes) {
    		 Question qustn = questionType.parse (unparsedQuestionString);
    		 if (qustn != null) {
    		    return qustn;
    	      }
      }
	  
	  throw new IllegalArgumentException();
  }

  /** Answer the freakin question! */
  public abstract String answer (LinkedList<String[][]> possibilitiesSet);

}

/* All the possible types of questions: */

class QWhatDoYouKnowAboutName extends Question {
	
	public QWhatDoYouKnowAboutName() { }
	
	public QWhatDoYouKnowAboutName(String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedQuestion = "What do you know about " + myName + "?";
	}
	
	public Question parse(String unparsedQuestionString) {
		
		String extractedArg = LineParser.extractMatch("What\\s*do\\s*you\\s*know\\s*about\\s*([A-Z][a-z]*)\\?", unparsedQuestionString);
		if (extractedArg != null) {
			return new QWhatDoYouKnowAboutName(extractedArg, null, null);
		} else return null;

	}
	
	public String answer (LinkedList<String[][]> possibilitiesSet) {
		String[] answer = Question.findAnswer(0, myName, possibilitiesSet);
		if (!answer[1].equals("#u") && !answer[2].equals("#u")) {
			return myName + " is the " + answer[1] + " and lives in the " + answer[2] + " house.";
		} else if (!answer[2].equals("#u")) {
			return myName + " lives in the " + answer[2] + " house.";
		} else if (!answer[1].equals("#u")) {
			return myName + " is the " + answer[1] + ".";
		} else {
			return "Nothing.";
		}
	}
}

class QWhatDoYouKnowAboutTheOccupation extends Question {
	
	public QWhatDoYouKnowAboutTheOccupation() {	}
	
	public QWhatDoYouKnowAboutTheOccupation(String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedQuestion = "What do you know about the " + myOccupation + "?";
	}
	
	public Question parse(String unparsedQuestionString) {
		
		String extractedArg = LineParser.extractMatch("What\\s*do\\s*you\\s*know\\s*about\\s*the\\s*([a-z]+)\\?", unparsedQuestionString);
		if (extractedArg != null) {
			return new QWhatDoYouKnowAboutTheOccupation(null, extractedArg, null);
		} else return null;
	}
	
	public String answer (LinkedList<String[][]> possibilitiesSet) {
		String[] answer = Question.findAnswer(1, myOccupation, possibilitiesSet);
		if (!answer[0].equals("#u") && !answer[2].equals("#u")) {
			return answer[0] + " is the " + myOccupation + " and lives in the " + answer[2] + " house.";
		} else if (!answer[2].equals("#u")) {
			return "The " + myOccupation + " lives in the " + answer[2] + " house.";
		} else if (!answer[0].equals("#u")) {
			return answer[0] + " is the " + myOccupation + ".";
		} else {
			return "Nothing.";
		}
	}
}

class QWhatDoYouKnowAboutTheColorHouse extends Question {
	
	public QWhatDoYouKnowAboutTheColorHouse() {	}
	
	public QWhatDoYouKnowAboutTheColorHouse(String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedQuestion = "What do you know about the " + myColor + " house?";
	}
	
	public Question parse(String unparsedQuestionString) {
		
		String extractedArg = LineParser.extractMatch("What\\s*do\\s*you\\s*know\\s*about\\s*the\\s*([a-z]+)\\s*house\\?", unparsedQuestionString);
		if (extractedArg != null) {
			return new QWhatDoYouKnowAboutTheColorHouse(null, null, extractedArg);
		} else return null;
		
	}
	
	public String answer (LinkedList<String[][]> possibilitiesSet) {
		String[] answer = Question.findAnswer(2, myColor, possibilitiesSet);
		if (!answer[0].equals("#u") && !answer[1].equals("#u")) {		//Have both
			return answer[0] + " is the " + answer[1] + " and lives in the " + myColor + " house.";
		} else if (!answer[1].equals("#u")) {	//Have an occupation
			return "The " + answer[1] + " lives in the " + myColor + " house.";
		} else if (!answer[0].equals("#u")) {	//Have name
			return answer[0] + " lives in the " + myColor + " house.";
		} else {
			return "Nothing.";
		}
	}
}

class QWhoIsTheOccupation extends Question {
	
	public QWhoIsTheOccupation() {	}
	
	public QWhoIsTheOccupation(String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedQuestion = "Who is the " + myOccupation + "?";
	}
	
	public Question parse(String unparsedQuestionString) {
		
		String extractedArg = LineParser.extractMatch("Who\\s*is\\s*the\\s*([a-z]+)\\?", unparsedQuestionString);
		if (extractedArg != null) {
			return new QWhoIsTheOccupation(null, extractedArg, null);
		} else return null;
		
	}
	
	public String answer (LinkedList<String[][]> possibilitiesSet) {
		String[] answer = Question.findAnswer(1, myOccupation, possibilitiesSet);
		if (!answer[0].equals("#u")) {
			return answer[0] + " is the " + myOccupation + ".";
		} else {
			return "I don't know.";
		}
			
	}
}

class QWhoLivesInTheColorHouse extends Question {
	
	public QWhoLivesInTheColorHouse() {	}
	
	public QWhoLivesInTheColorHouse(String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedQuestion = "Who lives in the " + myColor + " house?";
	}
	
	public Question parse(String unparsedQuestionString) {
		
		String extractedArg = LineParser.extractMatch("Who\\s*lives\\s*in\\s*the\\s*([a-z]+)\\s*house\\?", unparsedQuestionString);
		if (extractedArg != null) {
			return new QWhoLivesInTheColorHouse(null, null, extractedArg);
		} else return null;
	
	}
	
	public String answer (LinkedList<String[][]> possibilitiesSet) {
		String[] answer = Question.findAnswer(2, myColor, possibilitiesSet);
		if (!answer[0].equals("#u")) {
			return answer[0] + " lives in the " + myColor + " house.";
		} else {
			return "I don't know.";
		}
			
	}
}
class QWhatDoesTheOccupantOfTheColorHouseDo extends Question {
	
	public QWhatDoesTheOccupantOfTheColorHouseDo() { }
	
	public QWhatDoesTheOccupantOfTheColorHouseDo(String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedQuestion = "What does the occupant of the " + myColor + " house do?";
	}
	
	public Question parse(String unparsedQuestionString) {
		
		String extractedArg = LineParser.extractMatch("What\\s*does\\s*the\\s*occupant\\s*of\\s*the\\s*([a-z]+)\\s*house\\s*do\\?", unparsedQuestionString);
		if (extractedArg != null) {
			return new QWhatDoesTheOccupantOfTheColorHouseDo(null, null, extractedArg);
		} else return null;
		
	}
	
	public String answer (LinkedList<String[][]> possibilitiesSet) {
		String[] answer = Question.findAnswer(2, myColor, possibilitiesSet);
		if (!answer[1].equals("#u")) {
			return "The " + answer[1] + " lives in the " + myColor + " house.";
		} else {
			return "I don't know.";
		}
			
	}
}
class QWhatDoesNameDo extends Question {
	
	public QWhatDoesNameDo() {	}
	
	public QWhatDoesNameDo(String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedQuestion = "What does " + myName + " do?";
	}
	
	public Question parse(String unparsedQuestionString) {
		
		String extractedArg = LineParser.extractMatch("What\\s*does\\s*([A-Z][a-z]*)\\s*do\\?", unparsedQuestionString);
		if (extractedArg != null) {
			return new QWhatDoesNameDo(extractedArg, null, null);
		} else return null;
		
	}
	
	public String answer (LinkedList<String[][]> possibilitiesSet) {
		String[] answer = Question.findAnswer(0, myName, possibilitiesSet);
		if (!answer[1].equals("#u")) {
			return  myName + " is the " + answer[1] + ".";
		} else {
			return "I don't know.";
		}	
	}
}
class QWhereDoesNameLive extends Question {
	
	public QWhereDoesNameLive() { }
	
	public QWhereDoesNameLive(String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedQuestion = "Where does " + myName + " live?";
	}
	
	public Question parse(String unparsedQuestionString) {
		
		String extractedArg = LineParser.extractMatch("Where\\s*does\\s*([A-Z][a-z]*)\\s*live\\?", unparsedQuestionString);
		if (extractedArg != null) {
			return new QWhereDoesNameLive(extractedArg, null, null);
		} else return null;
		
	}
	
	public String answer (LinkedList<String[][]> possibilitiesSet) {
		String[] answer = Question.findAnswer(0, myName, possibilitiesSet);
		if (!answer[2].equals("#u")) {
			return  myName + " lives in the " + answer[2] + " house.";
		} else {
			return "I don't know.";
		}	
	}
}
class QWhereDoesTheOccupationLive extends Question {
	
	public QWhereDoesTheOccupationLive() {	}
	
	public QWhereDoesTheOccupationLive(String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedQuestion = "Where does the " + myOccupation + " live?";
	}
	
	public Question parse(String unparsedQuestionString) {
		
		String extractedArg = LineParser.extractMatch("Where\\s*does\\s*the\\s*([a-z]+)\\s*live\\?", unparsedQuestionString);
		if (extractedArg != null) {
			return new QWhereDoesTheOccupationLive(null, extractedArg, null);
		} else return null;
		
	}
	
	public String answer (LinkedList<String[][]> possibilitiesSet) {
		String[] answer = Question.findAnswer(1, myOccupation, possibilitiesSet);
		if (!answer[2].equals("#u")) {
			return  "The " + myOccupation + " lives in the " + answer[2] + " house.";
		} else {
			return "I don't know.";
		}	
	}
}
