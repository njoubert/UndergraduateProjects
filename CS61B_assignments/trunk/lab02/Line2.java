
import java.awt.Point;

public class Line2 {
	
    Point p1, p2;
	
    void printLength ( ) {
        double length;
        length = Math.sqrt ( (p2.getY() - p1.getY()) * (p2.getY() - p1.getY()) + (p2.getX() - p1.getX()) * (p2.getX() - p1.getX())  ) ;

        System.out.println ("Line length is " + length);
    }
	
    void printAngle ( ) {
        double angleInDegrees =  Math.atan2 ( (p2.getY() - p1.getY()), (p2.getX() - p1.getX())) * 180.0 / Math.PI;
        System.out.println ("Angle is " + angleInDegrees + " degrees");
    }

    public static void main(String[] args) {
        System.out.println ("testing Line2");
        Line2 myLine2 = new Line2 ();
        myLine2.p1 = new Point (5, 10);
        myLine2.p2 = new Point (45, 40);
        myLine2.printLength();
        myLine2.printAngle();

    }
}
