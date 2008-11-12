
public class Tests {

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		String blah = "b";
		String fromString = "abc";
		String toString = "ABC";
		System.out.println(fromString.subSequence(0, fromString.length()));
		System.out.println(blah.replace(fromString.subSequence(0, fromString.length()), toString.subSequence(0, toString.length())));
		System.out.println(blah);
	}

}
