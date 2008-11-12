import java.io.IOException;
import java.rmi.NotBoundException;

import ucb.util.mailbox.Mailbox;
import ucb.util.mailbox.QueuedMailbox;
import ucb.util.SimpleObjectRegistry;

class RemotePingPong {
	
	public static void main (String[] args) {
		String name, host, me;
		Mailbox<String> in, out;
		SimpleObjectRegistry reg;
		
		reg = null;    
		name = host = null;
		if (args.length == 0) {
			System.err.printf ("Usage: java RemotePingPong NAME%n  or  java RemotePingPong NAME HOST%n");
			System.exit (0);
		}
		
		name = args[0];
		
		if (args.length >= 2) {
			host = args[1];
			me = "client";
		} else 
			me = "host";
		
		in = out = null;
		
		if (host == null) {
			try {
				reg = new SimpleObjectRegistry ();
			} catch (IOException e) {
				System.err.println ("Could not create registry.%n");
				System.exit (1);
			}
			in = QueuedMailbox.<String>create (1);
			out = QueuedMailbox.<String>create (1);
			reg.rebind (name + ".IN", in);
			reg.rebind (name + ".OUT", out);
		} else {
			try {
				in = (Mailbox<String>) SimpleObjectRegistry.findObject (name + ".OUT", host);
				out = (Mailbox<String>) SimpleObjectRegistry.findObject (name + ".IN", host);
			} catch (NotBoundException e) {
				System.err.println ("Could not find remote mailboxes");
				System.exit (1);
			}
		}	
		
		try {
			for (int i = 0; i < 10; i += 1) {
				out.deposit (String.format ("%s %d", me, i));
				String msg = in.receive ();
				System.out.printf ("%s received message '%s'.%n", me, msg);
			}
			if (host == null)
			reg.close ();
			out.close (1000);
			//out.close ();
			
		} catch (IOException e) {
			System.err.printf ("%s encountered an IOException%n", me);
		} catch (InterruptedException e) { }
	}
	
}



