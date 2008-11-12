package game;

import java.io.IOException;

import ucb.util.mailbox.Mailbox;
import ucb.util.mailbox.QueuedMailbox;
import ucb.util.SimpleObjectRegistry;

/** A kind of Player that gets its commands from a remote program, and
 *  runs as a host of that remote program. 
 *  This player does no more than transpond messages between this
 *  program and a remote program - a link between two programs, nothing
 *  more.
 *  <P><H3>Handling remote communication</H3>
 *  <P>Since this player stands in for another computer, and
 *  acts as the host of the transaction, this player
 *  needs to set up the connection. 
 *  <br>Mailbox objects are used to store the messages being
 *  sent between computers.
 *  <br>SimpleObjectRegistry objects are used to communicate
 *  the remove invocation pointers between the host and
 *  the client.
 *  </P>
 *  <P> Upon instantiation of this object, two new mailboxes
 *  are created - "in" and "out" - which is where the
 *  messages will be read from and deposited. The "in" box
 *  represents the messages deposited by the client (thus the same
 *  as the client's "out" box) while the "out" box is where
 *  we deposit messages for the client (thus the same as the
 *  client's "in" box). These two objects are both
 *  added to a SimpleObjectRegistry, associated with the
 *  id given as argument to the constructor.
 *  The ClientPlayer uses this id (and the hostname) to find
 *  remote pointers to these mailboxes. </P>
 *  <P>The nextCommand() and sendCommand(String msg) simply
 *  deposits the appropriate messages into the appropriate
 *  boxes. Since the MailBox automatically "freezes" the current
 *  thread when it tries to access this mailbox and
 *  nothing is deposited there, we do not need to
 *  worry about the case where the other program hasn't 
 *  sent anything, except for a time-out if the other program
 *  disappears magically. </P>
 *  <H3>Error Handling</H3>
 *  <P>If anything goes wrong with the connection itself, we
 *  throw the necessary exceptions - IOExceptions as a general rule
 *  or their subclass RemoteException if something goes
 *  wrong with the connection. */

class HostPlayer extends Player {
	
	String type() {
		return "Host";
	}
	
	/** A Host for a game named ID on this machine.  If there 
	 *  is an error establishing contact, throws an IOException.
	 *  <br>This constructor creates and instantiates two actual
	 *  Mailbox objects, and creates the SimpleObjectRegistry with which
	 *  these mailboxes will be available to a remote client. 
	 *  @param id the id against which the mailboxes will be stored in the SimpleObjectRegistry.
	 *  @throws IOException if a connection could not be established to the host.
	 * */

	HostPlayer (String id) throws IOException {
		try {
			reg = new SimpleObjectRegistry ();
			in = QueuedMailbox.<String>create (1);
			out = QueuedMailbox.<String>create (1);
			reg.rebind (id + ".IN", in);
			reg.rebind (id + ".OUT", out);	
		} catch (IOException e) {
			throw new IOException("Could not create the registry on this host.");
		}
	}
	
	/** Reads the "in" mailbox for the command deposited there
	 *  by the remote machine. Timeouts after 10 seconds.
	 *  <BR>The actual recieving is all handled by the 
	 *  remote pointers, we simply call "in.recieve()" with 
	 *  a given timeout value.
	 * @throws IOException We throw an IOException with the appropriate message on a timeout or communication issue.
	 */
	String nextCommand () throws IOException {
		try {	
			String ret = in.receive();
			if (ret == null) {
				throw new IOException();
			}
			return ret;
		} catch (IOException e) {
			getGame().illegalRemoteOperation("We were unable to recieve a message.");
			return "";
		} catch (InterruptedException e) {
			throw new IOException ("The thread was interrupted before a message could be recieved.");
		}
	}
	
	/** Deposits the given message in the "out" box.
	 * <BR>The actual sending is handled by the remote
	 * pointers, we simply call out.deposit with the given
	 * message (and of course catch any errors).
	 * @throws IOException if anything in the communcation cycle goes wrong, 
	 * accompanied with the appropriate message in the error.*/
	void sendCommand(String msg) throws IOException {
		try {
			out.deposit(msg);
		} catch (IllegalStateException e) {
			getGame().illegalRemoteOperation("The remote connection has terminated unexpectedly.");
		} catch (IOException e) {
			throw new IOException ("We were unable to send the message to the remote computer. Is the client still running?");
		} catch (InterruptedException e) {
			throw new IOException ("The thread was interrupted before a message could be deposited.");
		}
	}
	
	
	/** The error handler to let this player know when an error occurred.
	 * We need not do anything with this message, 
	 * seeing that the actual display of errors are left
	 * to the different programs' "terminal" players, so we
	 * do not need to tell the other person an error occurred.
	 * (The remote messaging protocol also does not support this.
	 */
	void error (String msg) {}
	
	Player automatedPlayer () {
		close ();
		Player newMe = new MachinePlayer ();
		newMe.setGame (getGame ());
		newMe.setColor (getColor ());
		return newMe;
	}
	
	/** This should not be called, since there is no "manual mode" for
	 *  Player #2. */
	Player manualPlayer () {
		throw new UnsupportedOperationException ();
	}
	
	/** Execute any clean-up actions needed to close communications. 
	 * Closes both the mailboxes. Since it is quite possible
	 * for one or both of them to have been closed by the remote
	 * program already, we surround this in catch statements and ignore errors.*/
	void close () {
		try {
			reg.close();
			out.close();
			in.close();
		} catch (IOException e) {
			//IF WE ARE DONE AND SOMETHING GOES WRONG, IGNORE IT!
		} catch (InterruptedException e) { }
	}
	
	/** This method closes the registry 
	 * so that no other client can find this session 
	 * once a client has connected.
	 * Intended to be called when the first command
	 * is recieved from a client. */
	void closeRepository() {
		reg.close();
	}
	
	/** Flushes the mailbox. 
	 * Wait for all queued messages to be received, or MILLIS milliseconds, 
	 * whichever comes first. 
	 * During this wait, the Mailbox acts as if full 
	 * from the point of view of all threads attempting to deposit. */
	void flush(long millis) { 
		try {
			out.flush(millis);
		} catch (Exception e) {
			
		}
	}
	
	/** This holds the registry which allows the computers to find each other's objects */
	private SimpleObjectRegistry reg;
	/** Holds the mailboxes in which to deposit message or read messages.*/
	private Mailbox<String> in, out;
	/** How long we wait for anything to happen with the remote computer in milliseconds.*/
	static final int TIMEOUT = 300000;
	
}
