public class Line3 {
    
    int [ ] coords;
    
    void printLength ( ) {
		double length;
		length = Math.sqrt ( (coords[2] - coords[0]) * (coords[2] - coords[0]) + (coords[3] - coords[1]) * (coords[3] - coords[1]) ) ;
		System.out.println ("Line length is " + length);
    }
    
    void printAngle ( ) {
		double angleInDegrees = Math.atan2 ( (coords[3] - coords[1]) , (coords[2] - coords[0]) ) * 180.0 / Math.PI;
		System.out.println ("Angle is " + angleInDegrees + " degrees");
    }

    public static void main(String[] args) {
		System.out.println ("testing Line3");
		Line3 myLine = new Line3 () ;
		myLine.coords = new int [4] ;
		myLine.coords [0] = 5; myLine.coords [1] = 10; myLine.coords [2] = 45; myLine.coords [3] = 40;
		myLine.printLength();
		myLine.printAngle();
		// Set myLine to contain a reference to a new line object.
		// Initialize myLine's x1 and y1 (elements 0 and 1 of coords)
		// to the point (5, 10), and initialize myLine's x2 and y2 
		// (elements 2 and 3 of coords) to the point (45, 40).
		// Print the line's length, which should be 50.
		// Print the line's angle, which should be around 36.87 radians.
    }
}
