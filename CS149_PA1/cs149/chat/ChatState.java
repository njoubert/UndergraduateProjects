// ChatState
package cs149.chat;

import java.util.LinkedList;

/**
 * ChatState implements a multi-producer-consumer function. 
 * A limited buffer of MAX_HISTORY length is kept in the history list.
 * addMessage 
 * 	- appends a message to this list
 *  - removes messages beyond MAX_HISTORY count
 *  - Notify waiting threads of new messages
 * recentMessages 
 * 	- retrieves the current list of messages.
 *  - waits until SLEEP_TIMEOUT or new message arrives if no message is available.
 *  
 * @author njoubert
 *
 */
public class ChatState {
    private static final int MAX_HISTORY = 32;
    private static final int SLEEP_TIMEOUT = 15000;

    private final String name;
    private final LinkedList<String> history = new LinkedList<String>(); //youngest-to-oldest
    private long lastID = System.currentTimeMillis();

    public ChatState(final String name) {
        this.name = name;
        history.addLast("Hello " + name + "!");
    }

    public String getName() {
        return name;
    }

    //producer function
    //Only one producer at a time.
    //No consumers if a producer is busy.
    synchronized public void addMessage(final String msg) {
    	history.push(msg);
    	if (history.size() > MAX_HISTORY)
    		history.removeLast();
    	lastID++;
    	notifyAll();
    }

    //consumer function
    //right now this forces only one consumer at a time.
    //Really what you want is MANY consumers at a time, but they must all leave (or wait) if a producer is in there.
    synchronized public String recentMessages(long mostRecentSeenID) {
    	if (mostRecentSeenID == lastID) {
    		try {
				wait(SLEEP_TIMEOUT);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
    	}
    	int msgsCount = (int) ((int) (lastID - mostRecentSeenID) < history.size() ? lastID - mostRecentSeenID : history.size());
    	StringBuffer output = new StringBuffer();
    	for (int i = msgsCount - 1; i >= 0; i--) {
    		output.append(lastID - i + ": " + history.get(i) + "\n");
    	}
    	return output.toString();

    }
}
