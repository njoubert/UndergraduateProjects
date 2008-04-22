package org.joubert.mesh;

import java.util.Date;

public class Node {
 
    Date lastSeen;
    Date firstSeen;
    String hostname;
    String username;
    String userdesc;
    byte[] ipaddr;
 
    public Node(BroadcastDatagramPacket p) {
        hostname = p.getHostname();
        username = p.getUsername();
        userdesc = p.getUserdesc();
        ipaddr = p.getInetAddr().getAddress();
        lastSeen = new Date();
        firstSeen = new Date();
        
    }
    
    public void touch() {
        lastSeen = new Date();
    }
    
    @Override
    public boolean equals(Object obj) {
        Node other = (Node) obj;
        return (hostname.equals(other.hostname) &&
            username.equals(other.username) &&
            ipaddr[0] == other.ipaddr[0] &&
            ipaddr[1] == other.ipaddr[1] &&
            ipaddr[2] == other.ipaddr[2] &&
            ipaddr[3] == other.ipaddr[3]);
    }
    @Override
    public int hashCode() {
        return (hostname + username + ipaddr[0] + ipaddr[1] + ipaddr[2] + ipaddr[3]).hashCode();
    }




}
