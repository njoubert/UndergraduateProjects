
public class AccountTester {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		Account mike;
		mike = new Account (1000);
		System.out.println(mike.balance());
		if (! mike.deposit(100)) System.out.println("Sorry, your deposit failed!");
		System.out.println(mike.balance());
		if (! mike.withdraw(200)) System.out.println("Sorry, your withdrawal failed!");
		System.out.println(mike.balance());
		
		Account bob;
		bob = new Account (2000);
		System.out.println("Bob's Balance is now: " + bob.balance());
		mike.merge(bob);
		System.out.println("Bob's Balance is now " + bob.balance());
		System.out.println("while Mike's Balance is now " + mike.balance());
		
		System.out.println();
		Account kathy = new Account(500);
		Account megan = new Account(100, kathy);
		System.out.println("Megan's balance: " + megan.balance() + " Kathy's balance; " + kathy.balance());
		megan.withdraw(50);
		System.out.println("Megan's balance: " + megan.balance() + " Kathy's balance; " + kathy.balance());
		megan.withdraw(200);
		System.out.println("Megan's balance: " + megan.balance() + " Kathy's balance; " + kathy.balance());
		megan.withdraw(700);
		System.out.println("Megan's balance: " + megan.balance() + " Kathy's balance; " + kathy.balance());

		
		
	}

}
