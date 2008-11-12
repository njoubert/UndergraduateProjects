package temp;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;

import org.apache.hadoop.fs.Path;

public class KeywordFilter {
	protected static HashSet<String> stopWords;
	protected static boolean initialized = false;
	
	private static void initialize(Path stopPath) {
		if (stopPath == null)
			return;
		if (stopWords != null)
			return;
		stopWords = new HashSet<String>();
		try {
			BufferedReader readStopWords = new BufferedReader(new FileReader(stopPath.toString()));
			String stopword;
			while ((stopword = (readStopWords.readLine())) != null)
				stopWords.add(stopword.toLowerCase().trim());
		} catch (IOException e) {
			//intentionally left empty.
		}
		initialized = true; //we still initialize so that if there is no stopwords file, we can still run.
	}
	
	protected static String filter (String input, Path stopPath) {
		if (!initialized)
			initialize(stopPath);
		StringBuilder outputBuilder = new StringBuilder();
		//clean up word - lower case, only alphanumeric characters
		for (char i = 0; i < input.length(); i++) {
			if (Character.isLetterOrDigit(input.charAt(i)))
				outputBuilder.append(Character.toLowerCase(input.charAt(i)));
		}
		//check that it's still around, and not empty!
		if (outputBuilder.length() == 0)
			return "";
		String output = outputBuilder.toString();
		if ((stopWords == null) || !stopWords.contains(output))
			return output;
		return "";
	}
}