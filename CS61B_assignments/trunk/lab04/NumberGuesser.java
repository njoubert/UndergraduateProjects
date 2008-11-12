import java.io.*;

class NumberGuesser {

    public static void main (String [ ] args) throws IOException {
        BufferedReader keyboard = 
           new BufferedReader (new InputStreamReader (System.in));
        String answer;
	System.out.println ("Please think of an integer between 0 and 20 (inclusive).");
        int low, high, guess;
        low = 0;
        high = 20;
        guess = (low+high)/2;
        System.out.printf ("Is your number %d? (Type y or n.)%n", guess);
        answer = keyboard.readLine ( );
        while (!answer.equals ("y")) {
        	//at this point the answer doesn't equal y
        	//low and high are the endpoints of a range we guess in
        	//guess is the value between the high and the low range
            System.out.printf ("Is %d too high? (Type y or n.)%n", guess);
            answer = keyboard.readLine ( );
            if (answer.equals ("y")) {
                high = guess - 1;
            } else {
                low = guess + 1;
            }
            guess = (low+high)/2;
            //guess is the integer part of dividing low+high
            System.out.printf ("Is your number %d? (Type y or n.)%n", guess);
            answer = keyboard.readLine ( );
        }
    }
}
