import java.util.Scanner;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

public class Matching {
	public static void main (String[] args) {
		Scanner inp = new Scanner (System.in);
		inp.useDelimiter ("///\r?\n");
		System.out.println ("Enter QUIT/// to end the program.");
		while (true) {
			System.out.println ("One or more lines, ending with /// and a newline.");
			if (!inp.hasNext () || inp.hasNext ("QUIT"))
				break;
			String str = inp.next ();
			System.out.println ("Enter a pattern, ending with /// and a newline.");
			if (!inp.hasNext () || inp.hasNext ("QUIT"))
				break;
			String patn = inp.next ();
			Matcher mat = Pattern.compile (patn).matcher (str);
			if (mat.matches ()) {
				System.out.println ("Matches.");
				for (int i = 0; i <= mat.groupCount (); i += 1) {
					System.out.printf ("  Group %d: '%s'%n", 
							i, mat.group (i));
				}
			} else {
				System.out.println ("No match.");
			}
		}
	}
}
