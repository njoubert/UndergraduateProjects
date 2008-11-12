public class CheckDigit {

    public static void main (String [ ] args) {
	int id = 12345678;
	boolean isLegal = false;
	
	int n = 10;	// Digit Range container
	int sum = 0;	//Sum of digits container
	while (n < id) {
		sum = sum + ((id % (10 * n)) / n);
		n = n * 10;
	}
	if ((sum % 10) == (id % 10)) isLegal = true;
	
	if (isLegal) {
	    System.out.println (id + " is legal");
	} else {
	    System.out.println (id + " is not legal");
	}
    }
}
