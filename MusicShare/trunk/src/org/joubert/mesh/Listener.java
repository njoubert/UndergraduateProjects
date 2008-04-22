package org.joubert.mesh;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

import org.joubert.daemon.Main;

public class Listener implements Runnable {

    DatagramSocket recvSocket;
    
    public Listener() throws SocketException {
        recvSocket = new DatagramSocket(Main.getBroadcastPort());
        
    }
    
    public void run() {

        while (true) {
            byte[] recvBytes = new byte[256];
            DatagramPacket recvP = new DatagramPacket(recvBytes, recvBytes.length);
            
            try {
                recvSocket.receive(recvP);
                BroadcastDatagramPacket p = BroadcastDatagramPacket.makeFromDatagramPacket(recvP);
                
                
                System.out.println("Received notify from: " + p.getUsername());
                
                
                
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            
            
        }
    }

}
