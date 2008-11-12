public class Counter {

    private int myCount;

    public Counter ( ) {
        myCount = 0;
    }

    public void increment ( ) {
        myCount += 1;
    }

    public void reset ( ) {
        myCount = 0;
    }
    
    public int value ( ) {
        return myCount;
    }
}
