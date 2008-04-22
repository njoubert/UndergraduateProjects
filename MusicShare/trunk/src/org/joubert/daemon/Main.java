package org.joubert.daemon;

import java.net.SocketException;

import org.joubert.mesh.*;

/**
 * The main Daemon for the MusicSharing project.
 * <br/>It servers the following purposes:
 * <ul>
 *  <li>Broadcasts Presence on Network - UDP</li>
 *  <li>Keeps list of other nodes heard - UDP</li>
 *  <li>Indexes media library - Local</li>
 *  <li>Exposes interface to search through local library - TCP</li>
 * </ul>
 * @author Niels Joubert
 *
 */
public class Main {
    
    
    private static int port = 5668;

    private static Broadcaster bcaster;
    private static Listener listener;
    
    private static String name;
    
    /**
     * @param args
     */
    public static void main(String[] args) {
        
        if (args.length < 1) {
            System.out.println("Please supply your name as the first command line argument.");
            System.exit(1);
        }
            
       name = args[0]; 
       
       try {
        bcaster = new Broadcaster();
        listener = new Listener();
        
        
    } catch (SocketException e) {
        // TODO Auto-generated catch block
        e.printStackTrace();
    }
    
    Thread bc = new Thread(bcaster);
    bc.start();
    Thread ls = new Thread(listener);
    ls.start();
    
       
    }
    
    public static int getBroadcastPort() {
        return port;
    }

   public static String getUsername() {
       return name;
   }
}
