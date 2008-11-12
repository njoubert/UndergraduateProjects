package tracker;
import java.util.Scanner;
import java.io.FileInputStream;
import java.io.FileNotFoundException;

abstract class Command {

	   static private final Command[] commandTypes = {
		   new CommandBounds (),
		   new CommandAdd (),
		   new CommandRad (),
		   new CommandLoad (),
		   new CommandWrite (),
		   new CommandNear (),
		   new CommandWithin (),
		   new CommandSimulate (),
	       new CommandQuit (),
	       new CommandGUI()
	   };

	   /**
	    * Finds the right command to execute,
	    * attempts to build the command and execute it.
	    * Throws a ProblemException if this fails.
	    * @param command - Takes in the command string (one word, no args)
	    * @param in - the scanner from which the arguments will be read
	    * @return
	    * @throws ProblemException
	    */
	   public static String create (String commandText, Scanner inp, State theState) throws ProblemException, ParsedException {
	      if (commandText.equals("")) return "";
		   for (Command commandType : commandTypes) {
	    	  String cmnd = commandType.parse (commandText, inp, theState);
	    	  if (cmnd != null)
	    			 return cmnd;
	      }
	      throw new ParsedException(commandText, "unimplemented command");
	   }
	   
	   /**
	    * Extracts the amount of tokens given by the integer argument from the Scanner,
		* expecting each token to be a double, and returning an array of the
		* extracted tokens.
	    * @param in
	    * @param size
	    * @return
	    * @throws ProblemException
	    */
	   public static double[] extractArray (Scanner in, int size) throws ProblemException {
			double[] inputArray = new double[size];
			for (int i = 0; i < size; i++) {
				if (in.hasNextDouble()) {
					inputArray[i] = in.nextDouble();
				} else {
					throw new ProblemException("We expected a double as argument, but did not get it!");
				}
			}
			return inputArray;
	   }
		
	   abstract String parse (String commandText, Scanner inp, State theState) throws ProblemException, ParsedException;
	   
	   
}