/* Author: NIELS JOUBERT CS61B-BO */

/** Represents an assertion made in a sentence. */
abstract class Assertion {

	protected String myName;
	protected String myOccupation;
	protected String myColor;
	protected String storedAssertion;
	
	static private final Assertion[] assertionTypes = {
		  new ANameLivesInHouse(),
		  new ANameDoesNotLiveInHouse(),
		  new AOccupationLivesInHouse(),
		  new AOccupationDoesNotLiveInHouse(),
		  new ANameIsOccupation(),
		  new ANameIsNotOccupation(),
		  new ANameLivesAroundHere(),
		  new AOccupationLivesAroundHere(),
		  new AHouse()
	   };

	
  /** True iff the alleged fact that NAME is the OCCUPATION and lives in
   *  the COLOR house is not contradicted by THIS assertion.  Any one
   *  of NAME, OCCUPATION, or COLOR may be null, indicating an unknown
   *  value that does not match any non-null String. */
  abstract boolean allows (String name, String occupation, String color);

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

  /** A printable version of THIS.  Returns a sentence in the syntax
   *  defined by the puzzle problem statement, such as "Tom lives in the 
   *  white house." */
  public String toString () {
	    return storedAssertion;
	  }

	/** Returns a specific class if the given argument is valid */
  public abstract Assertion parse(String unparsedAssertionString);


  	/** Creates a specific Question class of a subtype of question */
  public static Assertion create(String unparsedAssertionString) throws IllegalArgumentException {
      
	  for (Assertion assertionType : assertionTypes) {
		  Assertion assertn = assertionType.parse (unparsedAssertionString);
    		 if (assertn != null) {
    		    return assertn;
    	      }
      }
	  
	  throw new IllegalArgumentException();
  }
}

class ANameLivesInHouse extends Assertion {
	
	public ANameLivesInHouse () { }
	
	/** Creates a new instance of this object. 
	 * If the needed argument info is not available, pass null as that argument.
	 */
	public ANameLivesInHouse (String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedAssertion = name + " lives in the " + color + " house.";
	}
	
	public Assertion parse(String unparsedAssertionString) {
		
		LineParser parser = new LineParser("([A-Z][a-z]*)\\s*lives\\s*in\\s*the\\s*([a-z]+)\\s*house.", unparsedAssertionString);
		if (parser.didMatch()) {
			String extractedName = parser.extractGroup(1);
			String extractedColor = parser.extractGroup(2);
			if (extractedName != null && extractedColor != null) {
				return new ANameLivesInHouse(extractedName, null, extractedColor);
			} else return null;
		} else {
			return null;
		}
	}
	
	public boolean allows (String name, String occupation, String color) {
		String[] expected = {myName, myColor};
		String[] given = {name, color};
		
		if (given[0].equals(expected[0]) && given[1].equals(expected[1])) {
			return true;
		} else if (given[0].equals(expected[0]) || given[1].equals(expected[1])) {
			return false;
		} else {
			return true;
		}

	}
}

class ANameDoesNotLiveInHouse extends Assertion {
	public ANameDoesNotLiveInHouse () { }
	
	/** Creates a new instance of this object. 
	 * If the needed argument info is not available, pass null as that argument.
	 */
	public ANameDoesNotLiveInHouse (String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedAssertion = name + " does not live in the " + color + " house.";
	}
	
	public Assertion parse(String unparsedAssertionString) {
		
		LineParser parser = new LineParser("([A-Z][a-z]*)\\s*does\\s*not\\s*live\\s*in\\s*the\\s*([a-z]+)\\s*house.", unparsedAssertionString);
		if (parser.didMatch()) {
			String extractedName = parser.extractGroup(1);
			String extractedColor = parser.extractGroup(2);
			if (extractedName != null && extractedColor != null) {
				return new ANameDoesNotLiveInHouse(extractedName, null, extractedColor);
			} else return null;
		} else {
			return null;
		}
	}
	
	public boolean allows (String name, String occupation, String color) {
			if (name.equals(myName) && color.equals(myColor)) {
				return false;
			} else if (name.equals(myName)) {
				return true;
			} else if (color.equals(myColor)) {
				return true;
			} else {
				return true;
			}

	}
	
}

class AOccupationLivesInHouse extends Assertion {
	
	public AOccupationLivesInHouse () { }
	
	/** Creates a new instance of this object. 
	 * If the needed argument info is not available, pass null as that argument.
	 */
	public AOccupationLivesInHouse (String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedAssertion = "The " + occupation + " lives in the " + color + " house.";
	}
	
	public Assertion parse(String unparsedAssertionString) {
		
		LineParser parser = new LineParser("The\\s*([a-z]+)\\s*lives\\s*in\\s*the\\s*([a-z]+)\\s*house.", unparsedAssertionString);
		if (parser.didMatch()) {
			String extractedOccupation = parser.extractGroup(1);
			String extractedColor = parser.extractGroup(2);
			if (extractedOccupation != null && extractedColor != null) {
				return new AOccupationLivesInHouse(null, extractedOccupation, extractedColor);
			} else return null;
		} else {
			return null;
		}
	}
	
	public boolean allows (String name, String occupation, String color) {
		
		String[] expected = {myOccupation, myColor};
		String[] given = {occupation, color};
		
		if (given[0].equals(expected[0]) && given[1].equals(expected[1])) {
			return true;
		} else if (given[0].equals(expected[0]) || given[1].equals(expected[1])) {
			return false;
		} else {
			return true;
		}
	}
}

class AOccupationDoesNotLiveInHouse extends Assertion {
	public AOccupationDoesNotLiveInHouse () { }
	
	/** Creates a new instance of this object. 
	 * If the needed argument info is not available, pass null as that argument.
	 */
	public AOccupationDoesNotLiveInHouse (String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedAssertion = "The " + occupation + " does not live in the " + color + " house.";
	}
	
	public Assertion parse(String unparsedAssertionString) {
		
		LineParser parser = new LineParser("The\\s*([a-z]+)\\s*does\\s*not\\s*live\\s*in\\s*the\\s*([a-z]+)\\s*house.", unparsedAssertionString);
		if (parser.didMatch()) {
			String extractedOccupation = parser.extractGroup(1);
			String extractedColor = parser.extractGroup(2);
			if (extractedOccupation != null && extractedColor != null) {
				return new AOccupationDoesNotLiveInHouse(null, extractedOccupation, extractedColor);
			} else return null;
		} else {
			return null;
		}
	}
	
	public boolean allows (String name, String occupation, String color) {
			if (occupation.equals(myOccupation) && color.equals(myColor)) {
				return false;
			} else if (occupation.equals(myOccupation)) {
				return true;
			} else if (color.equals(myColor)) {
				return true;
			} else {
				return true;
			}
	}
}

class ANameIsOccupation extends Assertion {
	public ANameIsOccupation () { }
	
	/** Creates a new instance of this object. 
	 * If the needed argument info is not available, pass null as that argument.
	 */
	public ANameIsOccupation (String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedAssertion = name + " is the " + occupation + ".";
	}
	
	public Assertion parse(String unparsedAssertionString) {
		
		LineParser parser = new LineParser("([A-Z][a-z]*)\\s*is\\s*the\\s*([a-z]+).", unparsedAssertionString);
		if (parser.didMatch()) {
			String extractedName = parser.extractGroup(1);
			String extractedOccupation = parser.extractGroup(2);
			if (extractedName != null && extractedOccupation != null) {
				return new ANameIsOccupation(extractedName, extractedOccupation, null);
			} else return null;
		} else {
			return null;
		}
	}
	
	public boolean allows (String name, String occupation, String color) {
			if (name.equals(myName) && occupation.equals(myOccupation)) {
				return true;
			} else if (name.equals(myName)) {
				return false;
			} else if (occupation.equals(myOccupation)) {
				return false;
			} else {
				return true;
			}
	}
}

class ANameIsNotOccupation extends Assertion {
	public ANameIsNotOccupation () { }
	
	/** Creates a new instance of this object. 
	 * If the needed argument info is not available, pass null as that argument.
	 */
	public ANameIsNotOccupation (String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedAssertion = name + " is not the " + occupation + ".";
	}
	
	public Assertion parse(String unparsedAssertionString) {
		
		LineParser parser = new LineParser("([A-Z][a-z]*)\\s*is\\s*not\\s*the\\s*([a-z]+).", unparsedAssertionString);
		if (parser.didMatch()) {
			String extractedName = parser.extractGroup(1);
			String extractedOccupation = parser.extractGroup(2);
			if (extractedName != null && extractedOccupation != null) {
				return new ANameIsNotOccupation(extractedName, extractedOccupation, null);
			} else return null;
		} else {
			return null;
		}
	}
	
	public boolean allows (String name, String occupation, String color) {
			if (name.equals(myName) && occupation.equals(myOccupation)) {
				return false;
			} else if (name.equals(myName)) {
				return true;
			} else if (occupation.equals(myOccupation)) {
				return true;
			} else {
				return true;
			}
	}
}

class ANameLivesAroundHere extends Assertion {
	public ANameLivesAroundHere () { }
	
	/** Creates a new instance of this object. 
	 * If the needed argument info is not available, pass null as that argument.
	 */
	public ANameLivesAroundHere (String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedAssertion = name + " lives around here.";
	}
	
	public Assertion parse(String unparsedAssertionString) {
		
		LineParser parser = new LineParser("([A-Z][a-z]*)\\s*lives\\s*around\\s*here.", unparsedAssertionString);
		if (parser.didMatch()) {
			String extractedName = parser.extractGroup(1);
			if (extractedName != null) {
				return new ANameLivesAroundHere(extractedName, null, null);
			} else return null;
		} else {
			return null;
		}
	}
	
	public boolean allows (String name, String occupation, String color) {
		return true;
	}
}

class AOccupationLivesAroundHere extends Assertion {
	public AOccupationLivesAroundHere () { }
	
	/** Creates a new instance of this object. 
	 * If the needed argument info is not available, pass null as that argument.
	 */
	public AOccupationLivesAroundHere (String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedAssertion = "The " + occupation + " lives around here.";
	}
	
	public Assertion parse(String unparsedAssertionString) {
		
		LineParser parser = new LineParser("The\\s*([a-z]+)\\s*lives\\s*around\\s*here.", unparsedAssertionString);
		if (parser.didMatch()) {
			String extractedOccupation = parser.extractGroup(1);
			if (extractedOccupation != null) {
				return new AOccupationLivesAroundHere(null, extractedOccupation, null);
			} else return null;
		} else {
			return null;
		}
	}
	
	public boolean allows (String name, String occupation, String color) {
		return true;
	}
}

class AHouse extends Assertion {
	public AHouse () { }
	
	/** Creates a new instance of this object. 
	 * If the needed argument info is not available, pass null as that argument.
	 */
	public AHouse (String name, String occupation, String color) {
		myName = name;
		myOccupation = occupation;
		myColor = color;
		storedAssertion = "There is a " + color + " house.";
	}
	
	public Assertion parse(String unparsedAssertionString) {
		
		LineParser parser = new LineParser("There\\s*is\\s*a\\s*([a-z]+)\\s*house.", unparsedAssertionString);
		if (parser.didMatch()) {
			String extractedColor = parser.extractGroup(1);
			if (extractedColor != null) {
				return new AHouse(null, null, extractedColor);
			} else return null;
		} else {
			return null;
		}
	}
	
	public boolean allows (String name, String occupation, String color) {
		return true;
	}
}
