import java.io.IOException;

import ucb.util.mailbox.Mailbox;
import ucb.util.mailbox.QueuedMailbox;

class PingPong extends Thread {

  public static void main (String[] args0) {
    try {
      Mailbox<String> pingsInBox = QueuedMailbox.<String>create (1);
      Mailbox<String> pongsInBox = QueuedMailbox.<String>create (1);
      //Mailbox<String> pingsInBox = QueuedMailbox.<String>create (10);
     // Mailbox<String> pongsInBox = QueuedMailbox.<String>create (10);

      PingPong ping = new PingPong ("Ping", pingsInBox, pongsInBox, 10);
      //PingPong pong = new PingPong ("Pong", pongsInBox, pingsInBox, 10);
      PingPong pong = new PingPong ("Pong", pongsInBox, pingsInBox, 5);

      ping.start ();
      pong.start ();

      //ping.join ();
      //pong.join ();
      ping.join (1000);
      pong.join (1000);
      ping.interrupt ();
    } catch (InterruptedException e) {
      System.err.println ("Main thread was interrupted.");
    }
  }

  PingPong (String name, Mailbox<String> in, Mailbox<String> out, int msgs) {
    super (name);
    this.name = name; this.in = in; this.out = out; this.msgs = msgs;
  }

  public void run () {
    try {
      for (int i = 0; i < msgs; i += 1) {
	out.deposit (String.format ("%s %d", name, i));
	String msg = in.receive ();
	System.out.printf ("%s received message '%s'.%n", name, msg);
      }
      out.close (1000);

    } catch (IOException e) {
      System.err.printf ("%s encountered an IOException%n", name);
    } catch (InterruptedException e) {
      System.err.printf ("%s has been interrupted.%n", name);
      try {
	out.close (1000);
      } catch (IOException e2) {
	System.err.printf ("%s encountered an IOException during close.%n", 
			   name);
      } catch (InterruptedException e2) {
	System.err.printf ("%s has been interrupted during close.%n", name);
      }
    }
  }

  private String name;
  private Mailbox<String> in, out;
  int msgs;

}


    
