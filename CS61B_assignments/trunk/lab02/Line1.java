public class Line1 {
	
    int x1, y1, x2, y2;
	
    void printLength ( ) {
        double length;
        length = Math.sqrt ( (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1 )) ;
        System.out.println ("Line length is " + length);
    }
	
    void printAngle ( ) {
        double angleInDegrees = Math.atan2 ( y2 - y1 , x2 - x1 ) * 180.0 / Math.PI;
        System.out.println ("Angle is " + angleInDegrees + " degrees");
    }

    public static void main(String [ ] args) {
        System.out.println ("testing Line1");
        Line1 myLine = new Line1 ();
        myLine.x1 = 5; myLine.x2 = 45; myLine.y1 = 10; myLine.y2 = 40;
        myLine.printLength();
        myLine.printAngle();
        // Set myLine to contain a reference to a new line object.
        // Initialize myLine's x1 and y1 to the point (5, 10),
        // and initialize myLine's x2 and y2 to the point (45, 40).
        // Print the line's length, which should be 50.
        // Print the line's angle, which should be around 36.87 degrees.
    }
}