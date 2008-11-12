import java.util.Scanner;
import java.io.File;
import java.io.FileNotFoundException;

public class shout {

   public static void main (String[] args) 
           throws FileNotFoundException {
      
	   File inputFile = new File(args[0]);
	   Scanner outputScanner = new Scanner(inputFile);
	   while (outputScanner.hasNext()) {
		   System.out.println(outputScanner.next().toUpperCase());
	   }
     // FILL THIS IN

   }

}

