package org.joubert.mesh;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.StringWriter;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Arrays;
import java.util.List;

import org.joubert.daemon.Main;

public class BroadcastDatagramPacket {
    
    private int port;
    private InetAddress ia;
    
    
    /**
     * Here follows the packet definition:
     */
    public static final int packetSize = 256; //256 total bytes
    public static final int HOSTNAMELEN = 64; //256 total bytes
    public static final int USERNAMELEN = 64; //256 total bytes
    public static final int USERDESCLEN = 128; //256 total bytes
    
    private byte[] signature;   //3 bytes
    private byte version;       //1 byte
    private BDPType type;       //4 bytes
    
    private String hostname;    //up to 64 bytes
    private String username;    //up to 64 bytes
    private String userdesc;    //up to 128 bytes
    
    public static enum BDPType {
        nothing(0x00),
        notify(0x01);
        
        private int bit;
        
        BDPType(int bitNumber) {
            bit = bitNumber;
        }
        public int getBitNumber() {
            return(bit);
        }
        public void setBitNumber(int i) {
            bit = i;
        }
        
    }
    
    private BroadcastDatagramPacket() throws UnknownHostException {
        // TODO Auto-generated constructor stub
        signature = new byte[3];
        signature[0] = 'm';
        signature[1] = 's';
        signature[2] = 'c';
        version = 1;      
        type = BDPType.nothing;
        hostname = InetAddress.getLocalHost().getHostName();
        username = Main.getUsername();
        userdesc = Main.getUserdesc();
        
    }
    
    public static BroadcastDatagramPacket makeNotify() throws UnknownHostException {
        
        BroadcastDatagramPacket ret = new BroadcastDatagramPacket();
        ret.ia = InetAddress.getByName("255.255.255.255");
        ret.port = Main.getBroadcastPort();
        ret.type = BDPType.notify;
        return ret;
    }
    
    public static BroadcastDatagramPacket makeFromDatagramPacket(DatagramPacket inPacket) throws IOException {
        BroadcastDatagramPacket ret = new BroadcastDatagramPacket();
        
        int start = inPacket.getOffset();
        int len = inPacket.getLength();
        byte[] data = inPacket.getData();
        
        if ((len - start) < BroadcastDatagramPacket.packetSize-1)
            System.out.println("Received packet of less than 255 bytes...");
        
        ret.ia = inPacket.getAddress();
        ret.port = inPacket.getPort();
        
        ByteArrayInputStream inS = new ByteArrayInputStream(data, start, len);
        DataInputStream in = new DataInputStream(inS);
        
        in.read(ret.signature, 0, 3);
        ret.version = in.readByte();
        ret.type.setBitNumber(in.readInt());
        byte[] host = new byte[HOSTNAMELEN];
        byte[] name = new byte[USERNAMELEN];
        byte[] desc = new byte[USERDESCLEN];
        
        in.read(host, 0, HOSTNAMELEN);
        in.read(name, 0, USERNAMELEN);
        in.read(desc, 0, USERDESCLEN);
        
        ret.username = (new String(name)).trim();
        ret.userdesc = (new String(desc)).trim();
        ret.hostname = (new String(host)).trim();
        
        return ret;
    }
   
    
    /**
     * Creates a datagram packet from the data stored in this object
     * @return a packet with filled buffer, ready to be sent.
     * @throws IOException 
     */
    public DatagramPacket getDatagramPacket() throws IOException {
        byte[] blanks = new byte[256];
        ByteArrayOutputStream outS = new ByteArrayOutputStream(BroadcastDatagramPacket.packetSize);
        DataOutputStream out = new DataOutputStream(outS);
        
        out.write(signature);
        out.write(version);
        out.writeInt(type.getBitNumber());
        out.writeBytes(hostname.substring(0, Math.min(HOSTNAMELEN-1, hostname.length())));
        out.write(blanks, 0, HOSTNAMELEN - Math.min(hostname.length(), HOSTNAMELEN-1));
        out.writeBytes(username.substring(0, Math.min(USERNAMELEN-1, username.length())));
        out.write(blanks, 0, USERNAMELEN - Math.min(USERNAMELEN-1, username.length()));
        out.writeBytes(userdesc.substring(0, Math.min(USERDESCLEN-1, userdesc.length())));
        out.write(blanks, 0, USERDESCLEN - Math.min(USERDESCLEN-1, userdesc.length()));
        
        byte[] buffer = outS.toByteArray();

        DatagramPacket ret = new DatagramPacket(buffer, buffer.length, ia, port);
        return ret;
    }

    public String getUsername() {
        return username.toString();
    }
    
    public String getUserdesc() {
        return userdesc.toString();
    }
    public String getHostname() {
        return hostname.toString();
    }

    public InetAddress getInetAddr() {
        return ia;
    }
}
