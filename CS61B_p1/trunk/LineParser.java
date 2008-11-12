import java.util.regex.Matcher;
import java.util.regex.Pattern;

class LineParser {
	
	private Matcher patternMatcher;
	/**
	 * Creates a new LineParser class, compiles the given pattern and stores the given string,
	 * and matches it to the given pattern.
	 * @param uncompiledPattern
	 * @param unparsedString
	 */
	public LineParser(String uncompiledPattern, String unparsedString) {
		patternMatcher = Pattern.compile(uncompiledPattern).matcher(unparsedString);
	}
	
	public boolean didMatch() {
		return patternMatcher.matches();
	}
	
	public String extractGroup(int group) throws IllegalArgumentException {
		String wantedGroup = patternMatcher.group(group);
		if (allowedWord(wantedGroup)) {
			return wantedGroup;
		} else {
			return null;
		}
	}
	
	static private final String[] unallowedKeywords = 
	{"who", "is", "the", "I", "don't", "know", "lives", "in", "color", "house",
			"what", "does", "name", "do", "occupation", "occupant", "of", "person-designator",
			"where", "you", "about", "nothing", "not", "live", "is", "not", "house-relation",
			"occupation-relation", "around", "here", "there"};
	
	/** extracts a single string using the given pattern from the given string.
	 * This also checks whether the given pattern conforms to the standards
	 */
	public static String extractMatch(String uncompiledPattern, String unparsedString) {
		Matcher mat = Pattern.compile(uncompiledPattern).matcher(unparsedString);
		if (mat.matches()) {
			if (allowedWord(mat.group(1))) {
				return mat.group(1);
			} else {
				return null;
			}
		} else {
			return null;
		}
	}
	/** Checks whether the given string is one of the unallowed keywords, and returns false if it is */
	private static boolean allowedWord(String word) {
		for (String keyword : unallowedKeywords) {
			if (keyword.equals(word.toLowerCase())) return false;
		}
		return true;
	}
	
}
