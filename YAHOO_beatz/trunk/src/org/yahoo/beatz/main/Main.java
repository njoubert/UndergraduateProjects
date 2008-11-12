package org.yahoo.beatz.main;

import java.io.File;

public class Main {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub

		File crazy = new File("Everything.wav");
		if (!crazy.exists())
			System.out.println("File does not exist...");
		
		BeatsAnalyzer b = new BeatsAnalyzer(crazy, false, true, true);
	}

}
