// ChatState
package cs149.chat;

import java.util.LinkedList;

public class ChatState {
    private static final int MAX_HISTORY = 32;

    private final String name;
    private final LinkedList<String> history = new LinkedList<String>();
    private long lastID = System.currentTimeMillis();

    public ChatState(final String name) {
        this.name = name;
        history.addLast("Hello " + name + "!");
    }

    public String getName() {
        return name;
    }

    public void addMessage(final String msg) {
    }

    public String recentMessages(long mostRecentSeenID) {
        return null;
    }
}
