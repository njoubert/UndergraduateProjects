package org.joubert.mesh;

import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Arrays;
import java.util.List;

import org.joubert.daemon.Main;

public class BroadcastDatagramPacket {

    private int port;
    private InetAddress ia;
    private BDPType type;
    private byte version;
    private String packetdesc;
    private String username;
    private String userdesc;
    
    public static enum BDPType {
        notify(0x01);
        
        private int bit;
        
        BDPType(int bitNumber) {
            bit = bitNumber;
        }
        public int getBitNumber() {
            return(bit);
        }
        
    }
    
    private BroadcastDatagramPacket() {
        // TODO Auto-generated constructor stub
        port = Main.getBroadcastPort();
        version = 1;
    }
    
    public static BroadcastDatagramPacket makeNotify() {
        
        //We need to take into account network byte order!
        
        BroadcastDatagramPacket ret = new BroadcastDatagramPacket();
        try {
            ret.ia = InetAddress.getByName("255.255.255.255");
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }
        ret.type = BDPType.notify;
        ret.packetdesc = "MusicShare Broadcast Notify";
        ret.username = Main.getUsername();
        ret.userdesc = "The Main Node";
        return ret;
    }
    
    public static BroadcastDatagramPacket makeFromDatagramPacket(DatagramPacket in) {
        BroadcastDatagramPacket ret = new BroadcastDatagramPacket();
        
      //We need to take into account network byte order!
        
        int start = in.getOffset();
        int len = in.getLength();
        byte[] data = in.getData();
        
        if ((len - start) < 255)
            System.out.println("Received packet of less than 255 bytes...");
        
        byte usernameLen = data[61];
        byte userDescLen = data[62];
        
        ret.version = data[63];
        ret.username = new String(data, 64, Math.min(64, usernameLen));
        ret.userdesc = new String(data, 128, Math.min(128, userDescLen));
        
        
        return ret;
    }
    
    /**
     * Creates a datagram packet from the data stored in this object
     * @return
     */
    public DatagramPacket getDatagramPacket() {
        byte[] buf = new byte[256];
        byte[] first = packetdesc.getBytes();
        byte[] second = username.getBytes();
        byte[] third = userdesc.getBytes();
        System.arraycopy(first, 0, buf, 0, Math.min(60, first.length));
        buf[60] = version;
        buf[61] = (byte) Math.min(64, second.length);
        buf[62] = (byte) Math.min(128, third.length);
        System.arraycopy(second, 0, buf, 64, buf[61]);
        System.arraycopy(third, 0, buf, 128, buf[62]);
        
        DatagramPacket ret = new DatagramPacket(buf, buf.length, ia, port);
        return ret;
    }
    
    
    public String getUsername() {
        return username;
    }
    
    public String getUserdesc() {
        return userdesc;
    }

}
