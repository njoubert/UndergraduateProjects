
public class Account {

    // This class represents a bank account whose current
    // balance is a nonnegative amount in US dollars.
        
    // Initialize an account with the given balance. 
    public Account (int balance) {
        myBalance = balance;
        parentAccount = null;
    }
    
    public Account (int balance, Account parent) {
        myBalance = balance;
        parentAccount = parent;
    }
    
    // Add the given amount to the account.
    public boolean deposit (int amount) {
        if (amount < 0) {
            System.out.println ("Cannot deposit a negative amount.");
            return false;
        } else {    
            myBalance = myBalance + amount;
            return true;
        }
    }
    
    // Subtract the given amount from the account
    // if possible.  If the amount would leave a
    // negative balance, print an error message and
    // leave the balance unchanged.
    public boolean withdraw (int amount) {
    	/*
    	 * Either subtracts the required amount from
    	 * a) this account
    	 * b) this account and the parent account
    	 * and returns true
    	 * or
    	 * returns false
    	 */
        if (amount < 0) {
            System.out.println ("Cannot withdraw a negative amount.");
            return false;
        } else if (myBalance < amount) {
        	if (parentAccount != null && parentAccount.withdraw(amount - this.balance())) {
        		myBalance = 0;
        		return true;
        	} else {
        		System.out.println("Insufficiant funds in any account!");
        		return false;
        	}
        } else {
            myBalance = myBalance - amount;
            return true;
        }
    }
    
    // Merge account "anotherAcct" into this one by 
    // removing all the money from anotherAcct and 
    // adding that amount to this one.
    public void merge (Account anotherAcct) {
        this.deposit(anotherAcct.balance());
        anotherAcct.withdraw(anotherAcct.balance());
    }
    
    // Return the number of dollars in the account.
    public int balance ( ) {
        return myBalance;
    }

    private int myBalance;
    private Account parentAccount;
}