/* Author: NIELS JOUBERT CS61B-BO */

import java.util.List;
import java.util.LinkedList;
import java.util.Iterator;

/** A puzzle-solving engine */
class PuzzleSolver {

  /** A solver that answers questions about a puzzle defined by ASSERTIONS,
   */
	
	public LinkedList<int[][]> possibilities = new LinkedList();	//This is for the permutations
	public LinkedList<String[][]> answers = new LinkedList();		//This is for the associated answers
	
	private LinkedList<String> names = new LinkedList();
	private LinkedList<String> occupations = new LinkedList();
	private LinkedList<String> colors = new LinkedList();
	
	private boolean isPossible = true;
	
	private LinkedList<Assertion> assertions;
	

	
  PuzzleSolver (LinkedList<Assertion> asserts) {
	  assertions = asserts;
	  extractKeywords();	/*Extracts the keywords from the assertions
	  						* and fills up the names, occupations, colors correctly. */
	  createAnonymous();	//Makes sure that you have the same amount of everything by filling in nulls.
	  
	  createPossibilities(); /*Fills up the list of possibilities using the permutation generator
	  						* and filters it against all the assertions.
	  						* this depends on the names / occupations / colors being set */
  }
  
  /** extractKeywords runs through the assetions and extracts keywords.
   * It also makes sure that you have the same amount of keywords be generating
   * anonymous ones.
   *
   */
  
  /** extracts keywords from the assertion list. This also checks that
   * no name, occupation or color occurs in more than one type.
   */
  void extractKeywords() {
	  Iterator<Assertion> assertionIterator = assertions.listIterator(0);
	  while (assertionIterator.hasNext()) {
		  Assertion currentAssert = assertionIterator.next();
		  
		  String newName = currentAssert.getName();
		  String newOccupation = currentAssert.getOccupation();
		  String newColor = currentAssert.getColor();
		  
		  //Checks to make sure that there's no duplicates
		  
		  if (newName != null) {
			  if (occupations.contains(newName) ||
					  colors.contains(newName) ||
					  newName == newColor ||
					  newName == newOccupation) {
				  throw new IllegalArgumentException("newName already in list " + newName);
			  }
		  }
		  if (newOccupation != null) {
			  if (names.contains(newOccupation) ||
					  colors.contains(newOccupation) ||
					  newOccupation == newColor ||
					  newOccupation == newName) {
				  throw new IllegalArgumentException("newOccupation already in list " + newOccupation);
			  }
		  }
		  if (newColor != null) {
			  if (names.contains(newColor) ||
					  occupations.contains(newColor) ||
					  newColor == newName ||
					  newColor == newOccupation) {
				  throw new IllegalArgumentException("newColor already in list " + newColor);
			  }
		  }
		  addUniqueString(names, newName);
		  addUniqueString(occupations, newOccupation);
		  addUniqueString(colors, newColor);  
	  }
	  
  }
  
  /** This class figures out what the maximum number of names/colors/occupations there is
   * and adds the proper amount of nulls to those that are less than this.
   */
  void createAnonymous() {
	  int totalThings = Math.max(names.size(), Math.max(colors.size(), occupations.size()));
	  for (int i = totalThings - names.size(); i > 0; i--) {
		  names.add("#u");
	  }
	  for (int i = totalThings - occupations.size(); i > 0; i--) {
		  occupations.add("#u");
	  }
	  for (int i = totalThings - colors.size(); i > 0; i--) {
		  colors.add("#u");
	  }
  }
  /** This method fills up the list of possibilities of this class.
   * It creates a PermutationsPairs class with the length of the names field
   * as the total permutations.
   * We then sequentially extracts pairs:
   * 	Get a pair, and save it as our current possibilitiesPair.
   * 	Get the names/colors/occupations association of this pair with AassociatePermsWithStrings()
   * 	We now match up every assertion against this possibility:
   * 		Match against the assertion.
   * 		if false - set currentPosibilitiesPair = null and break
   * 	
   * 	if the currentPossibilitiesPair is not null, it has passed all the assertions!
   * 	add it to the list of possibilities!
   * Do again with next pair;
   * 	
   *
   */
  void createPossibilities() {
	  
	  PermutationPairs totalPossibilities = new PermutationPairs(names.size());
	  int[][] currentPossibility;	//The integer array representation of the current possibility
	  String[][] associatedTriples; //The string array of triples representing the current possilibity
	  
	  Assertion currentAssertion;
	  Iterator<Assertion> assertionsIterator;
	  
	  boolean validSolution = true;
	  
	  while (totalPossibilities.hasNextPair()) {	//Match every possibility
		  
		  currentPossibility = totalPossibilities.nextPair().clone();
		  associatedTriples = associatePermsWithStrings(currentPossibility);
		  assertionsIterator = assertions.iterator();
		  validSolution = true;
		  
		  while (assertionsIterator.hasNext()) {	//Test against every assertion
			  
			  currentAssertion = assertionsIterator.next();
			  
			  for (int i = 0; i < associatedTriples.length; ++i) {	//Test each triple of assertion against possibility
				  if (!currentAssertion.allows(associatedTriples[i][0], associatedTriples[i][1], associatedTriples[i][2])) {
					  validSolution = false;
					  break;
				  }
			  }
			  if (validSolution == false) break;
		  }
		  
		  if (validSolution == true) {	//If it survived the tests, we add
			  possibilities.add(currentPossibility);
			  answers.add(associatedTriples);
		  }
		  
	  }
	  
	  if (possibilities.size() == 0) {
		  isPossible = false;
	  }
	  
	  
  }
  
  boolean isPossible() {
	  return isPossible;
  }

  /** The answer to QUERY.  Returns null if the assertions used to 
   *  construct THIS are inconsistent. This also makes sure
   *  that the question doesnt mention something we don't know about.*/
  String answer (Question query) {
	  if (crossCheck(query)) {
		  return query.answer(answers);
	  }
	  solve.exit(1);
	  return null;
  }
  
  /** checks whether the given question conforms to the known names/occupations/colors */
  private boolean crossCheck(Question query) {
	  String name = query.getName();
	  String occupation = query.getOccupation();
	  String color = query.getColor();
	  String temp = null;
	  
	  Iterator<String> iter;
	  
	  if (name != null) {
		  boolean validName = false;
		  iter = names.iterator();
		  while(iter.hasNext()) {
			  if (iter.next().equals(name)) {
				  validName = true;
			  }
		  }
		  return validName;
	  }
	  
	  if (occupation != null) {
		  boolean validOccupation = false;
		  iter = occupations.iterator();
		  while(iter.hasNext()) {
			  if (iter.next().equals(occupation)) {
				  validOccupation = true;
			  }
		  }
		  return validOccupation;
	  }

	  if (color != null) {
		  boolean validColor = false;
		  iter = colors.iterator();
		  while(iter.hasNext()) {
			  temp = iter.next();
			  if (temp.equals(color)) {
				  validColor = true;
			  }
		  }
		  return validColor;
	  }

	  return false;
	  
  }
  
  public LinkedList<String> getNames() { return names;}
  
  public LinkedList<String> getOccupations() { return occupations; }
  
  public LinkedList<String> getColors() { return colors; }

  /**
   * This method will take in a permutation pair and match
   * it up against the stored names, colors, occupations, and return
   * an array of Strings, with n arrays each consisting of a name, occupation, color triple.
   * nulls are translated to #u.
   */
  	String[][] associatePermsWithStrings(int[][] PermPair) {
  		
  		int n = PermPair[0].length;	//We depend on the fact that there is a pair of arrays each of the same length.
  		if (n > names.size()) return null; //Check to make sure input is valid
  		String[][] triples = new String[n][3];	//n amount of triples.
  		
  		for (int i = 0; i < n; ++i) {
  			triples[i][0] = names.get(i);
  			if (triples[i][0] == null) triples[i][0] = "#u";
  			
  			triples[i][1] = occupations.get(PermPair[0][i]);
  			if (triples[i][1] == null) triples[i][1] = "#u";
  			
  			triples[i][2] = colors.get(PermPair[1][i]);
  			if (triples[i][2] == null) triples[i][2] = "#u";
  		}
  		
  		return triples;
  	}
  	
    /** adds the element only to the list if the element is unique. 
     * Does NOT add null!
     * */
    void addUniqueString(List<String> ls, String s)  {
  	  if (s != null && !ls.contains(s)) {
  		  ls.add(s);
  	  }
    }
}
