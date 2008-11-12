import junit.framework.TestCase;

public class BankAccountTest extends TestCase {
	
	public void testInit() {
		BankAccount froober = new BankAccount(1000);
		assertEquals(1000, froober.balance());
	}
	
	public void testInvalidArgs() {
		BankAccount froober = new BankAccount(1000);
		froober.deposit(-100);
		assertEquals(1000, froober.balance());
		froober.withdraw(-100);
		assertEquals(1000, froober.balance());
	}
	
	public void testOverdraft() {
		BankAccount froober = new BankAccount(1000);
		froober.withdraw(1000000);
		assertEquals(1000, froober.balance());
	}
	
	public void testDeposit() {
		BankAccount froober = new BankAccount(1000);
		froober.deposit(100);
		assertEquals(1100, froober.balance());
	}
	
	public void testWithdraw() {
		BankAccount froober = new BankAccount (1000);
		froober.withdraw(100);
		assertEquals(900, froober.balance());
	}
}
