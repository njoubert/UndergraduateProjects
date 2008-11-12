package game;

import java.io.IOException;
import java.rmi.NotBoundException;


import ucb.util.mailbox.*;
import ucb.util.SimpleObjectRegistry;

/** A kind of Player that gets its commands from a remote program, and
 *  runs as a client of that remote program. 
 *  This player does no more than transpond messages between this
 *  program and a remote program - a link between two programs, nothing
 *  more.
 *  <P><H3>Handling remote communication</H3>
 *  <P>Since this player stands in for another computer, and
 *  acts as the client of the transaction, this player
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
 *  client's "in" box).
 *  <BR>The <b>creation of these mailboxes</b> occur by using the 
 *  SimpleObjectRegistry to retrieve remote pointers to
 *  the host's Mailbox objects, which is stored as mailboxes 
 *  associated with their respective in and out mailbox variables.
 *  Notice how our "in" box corresponds to the host's "out" box and
 *  vice versa - this is a logical ordering to get messages to each other
 *  by always sending to "out" and recieving from "in".
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

class ClientPlayer extends Player {
	
	String type() {
		return "Client";
	}
	/** A Client of the game named ID on the host HOST. If there is an 
	 *  error establishing contact, throws an IOException. 
	 *  <br>This constructor creates the two mailboxes - "in" and "out" - 
	 *  and retrieves (or attempts to retrieve) remote pointers to the
	 *  host's Mailboxes. If this fails, an IOException error is signalled
	 *  @param host the hostname as string
	 *  @param id the id of the mailboxes as stored in the SimpleObjectRegistry of the host.
	 *  @throws IOException if a connection could not be established to the host.
	 * 
	 * */
	
	@SuppressWarnings("unchecked") ClientPlayer (String host, String id) throws IOException {
		try {
			in = (Mailbox<String>) SimpleObjectRegistry.findObject (id + ".OUT", host);
			out = (Mailbox<String>) SimpleObjectRegistry.findObject (id + ".IN", host);
		} catch (NotBoundException e) {
			throw new IOException ("Could not find remote mailboxes on the host.");
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
			getGame().illegalRemoteOperation("We were unable to recieve a message within the allotted time. Is the host still running?");
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
			throw new IOException ("We were unable to send the message to the remote computer. Is the host still running?");
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
			
			out.close();
			in.close();
		} catch (IOException e) {
			//IF WE ARE DONE AND SOMETHING GOES WRONG, IGNORE IT!
		} catch (InterruptedException e) { }
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
	
	/** Holds the mailboxes in which to deposit message or read messages.*/
	private Mailbox<String> in, out;
	/** How long we wait for anything to happen with the remote computer in milliseconds.*/
	static final int TIMEOUT = 3000;
}
