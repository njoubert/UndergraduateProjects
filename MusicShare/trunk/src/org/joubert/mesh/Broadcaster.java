package org.joubert.mesh;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.DatagramSocketImpl;
import java.net.SocketException;

/**
 * Broadcasts this computer's presence on the network.
 * @author Niels Joubert
 *
 */
public class Broadcaster implements Runnable {

    boolean doBroadcast;
    boolean pleaseStop;
    
    DatagramSocket sendSocket;
    
    public Broadcaster() throws SocketException {
        doBroadcast = true;
        pleaseStop = false;
        sendSocket = new DatagramSocket();
        sendSocket.setBroadcast(true);
    }
    
    public void run() {
        // TODO Auto-generated method stub
        while (!pleaseStop) {
            if (doBroadcast) {
                BroadcastDatagramPacket p = BroadcastDatagramPacket.makeNotify();
                broadcastPacket(p);
            }
            
            try {
                Thread.sleep(5000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
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
    
    private void broadcastPacket(BroadcastDatagramPacket p) {
        //System.out.println("Broadcasting... " + p.hashCode());
        
        DatagramPacket toSend = p.getDatagramPacket();
        try {
            sendSocket.send(toSend);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

}
