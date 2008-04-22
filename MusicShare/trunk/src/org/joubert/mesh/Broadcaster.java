package org.joubert.mesh;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.DatagramSocketImpl;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.ArrayList;

import org.joubert.daemon.Main;

/**
 * Broadcasts this computer's presence on the network.
 * @author Niels Joubert
 *
 */
public class Broadcaster implements Runnable {

    boolean doBroadcast;
    boolean pleaseStop;
    private long lastBroadcast;
    
    ArrayList<BroadcastDatagramPacket> queue = new ArrayList<BroadcastDatagramPacket>();
    
    DatagramSocket sendSocket;
    
    public Broadcaster() throws SocketException {
        doBroadcast = true;
        pleaseStop = false;
        sendSocket = new DatagramSocket();
        sendSocket.setBroadcast(true);
        lastBroadcast = 0;
    }
    
    /**
     * Sends the queue of packets, possibly sending our presence. Then sleeps for a moment, before retrying.
     */
    public void run() {
        
        try {
        
                while (!pleaseStop) {
              
                    broadcastPresence();
                    
                    while (!queue.isEmpty() && doBroadcast) {   
                        broadcastPacket(queue.remove(0));
                    }
                    
                    try {
                        Thread.sleep(200);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }    
                }
                
                sendSocket.close();
        
        } catch (IOException e) {
            e.printStackTrace();
        }
        
    }
    
    /**
     * Will broadcast out presence if enough time has elapsed since the last broadcast.
     * @throws UnknownHostException 
     */
    private void broadcastPresence() throws UnknownHostException {
        long now = System.currentTimeMillis();
        int broadcastGranularity = Integer.parseInt(Main.properties.getProperty("broadcastGranularitySeconds")) * 1000;
        if (now - lastBroadcast > broadcastGranularity) {
            BroadcastDatagramPacket p = BroadcastDatagramPacket.makeNotify();
            queue.add(p);
            lastBroadcast = now;
        }
    }

    synchronized public void pause() {
        doBroadcast = false;
    }
    /**
     * @return True if successfully unpaused.
     */
    synchronized public boolean unpause() {
        if (pleaseStop)
            return false;
        doBroadcast = true;
        return true;
    }
    synchronized public void stop() {
        pleaseStop = true;
    }
    
    private void broadcastPacket(BroadcastDatagramPacket p) throws IOException {
        
        DatagramPacket toSend = p.getDatagramPacket();
        try {
            sendSocket.send(toSend);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

}
