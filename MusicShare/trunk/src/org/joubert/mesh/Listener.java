package org.joubert.mesh;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.ArrayList;

import org.joubert.daemon.Main;

public class Listener implements Runnable {

    DatagramSocket recvSocket;
    public ArrayList<Node> nodes;
    
    public Listener() throws SocketException {
        recvSocket = new DatagramSocket(Main.getBroadcastPort());
        nodes = new ArrayList<Node>();
        
    }
    
    public void run() {

        while (true) {
            byte[] recvBytes = new byte[256];
            DatagramPacket recvP = new DatagramPacket(recvBytes, recvBytes.length);
            
            try {
                
                recvSocket.receive(recvP);
                BroadcastDatagramPacket p = BroadcastDatagramPacket.makeFromDatagramPacket(recvP);
                handleReceivedPacket(p);
                
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            
            
        }
    }

    private void handleReceivedPacket(BroadcastDatagramPacket p) {
        
        
        Node newNode = new Node(p);
        int n;
        if ((n = nodes.indexOf(newNode)) == -1) {
            
            nodes.add(newNode);
            //Here we can insert a callback if neccesary
            
        } else {
            newNode = nodes.get(n);
        }
        newNode.touch();
        
        
        
    }
    

}
