package game;

/** Player and square colors for jump61b.  To conform to Java convention
 *  that constants, including enumerals, be upper-cased, the parseColor 
 *  method ignores case and toString uses lower-case.  */
enum Color {
	
	WHITE, RED, BLUE;
	
	/** The reverse of THIS Color: Blue for Red, Red for Blue, White for
	 *  White. */
	Color opposite () {
		return opposites[ordinal ()];
	}
	
	/** True iff it is legal for the player of THIS Color to play on 
	 *  a square of Color COLOR. */
	boolean playableSquare (Color color) {
		return color == WHITE || color == this;
	}
	
	/** The color named COLORNAME, ignoring case differences (convenience
	 *  method). */
	static Color parseColor (String colorName) {
		return valueOf (colorName.toUpperCase ());
	}
	
	/**
	 * Returns the Color with the same ordinal vaue as the given int
	 * @param cVal - The ordinal value of a color
	 * @return Color - the color associated with the given value
	 */
	static Color parseColor (int cVal) {
		return Color.values()[cVal];
	}
	
	/** The lower-case name of THIS. */
	public String toString () {
		return super.toString ().toLowerCase ();
	}
	
	public String toWinnerString() {
		String out = super.toString().toLowerCase();
		out = out.replace('r', 'R');
		out = out.replace('b', 'B');
		return out;
	}
	
	/** Returns a small representation of this - only a single letter, {r, w, b} */
	public String toStringSmall() {
		return super.toString().toLowerCase().substring(0, 1);
	}
	
	public java.awt.Color toAWTColor() {
		return dispColors[this.ordinal()];
	}
	
	/** Color of Red player's squares. */
	private static java.awt.Color redSquareColor = new java.awt.Color(255, 150, 150);
	/** Color of Blue player's squares. */
	private static java.awt.Color blueSquareColor = new java.awt.Color(150, 150, 255);
	/** Color of White squares */
	private static java.awt.Color whiteSquareColor = new java.awt.Color(250, 255, 250);
	
	private static Color[] opposites = { WHITE, BLUE, RED };
	@SuppressWarnings("unused")
	private static String[] names = { "white", "red", "blue" };
	private static java.awt.Color[] dispColors = {whiteSquareColor, redSquareColor, blueSquareColor };
	
}



