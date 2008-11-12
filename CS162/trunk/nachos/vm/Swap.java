package nachos.vm;

import java.util.Hashtable;
import java.util.LinkedList;
import nachos.machine.Machine;
import nachos.machine.OpenFile;
import nachos.machine.Processor;
import nachos.threads.Lock;
import nachos.threads.TestStubs;
import nachos.threads.ThreadedKernel;
import nachos.vm.MemoryMap.MemoryMapEntry;

public class Swap {

    private static final String swapFileName = "swapFile.dat";
    
    private static int swapInCount=0;
    private static int swapOutCount=0;
    private static int lastSwapFileSize;
    
    private Lock swapLock;
    private OpenFile swapFile;
    private LinkedList<Integer> freeList;
    private int swapFileSize;
    private Hashtable<Integer, Hashtable<Integer, Integer>> swapFileTable; //Maps PID,VPN to 
    
    /**
     * Returns true if the given virtual page is in the swap file.
     * @return offset in pages if we contain this page, else -1;
     */

    public void close() {
        swapFile.close();
        ThreadedKernel.fileSystem.remove(swapFileName);
        lastSwapFileSize = swapFileSize;
        Swap.theOnlySwap = null;
    }
    
    public boolean contains(int pid, int vpn) {
        swapLock.acquire();
        boolean has = (getOffsetOfPage(pid,vpn) >= 0);
        swapLock.release();
        return has;
    }
    
    /**
     * Swaps out a page, modifies main memory.
     * MODIFIES the given entry - entry becomes INVALID!
     * @param entry
     * @return true on success
     */
    public boolean moveToSwap(MemoryMapEntry entry) {
        
        TestStubs.debugPrint("MoveToSwap " + entry.toString());
        
        if (!entry.isPinned()) {
            TestStubs.assertTrue(false);
            return false;
        }
        
        swapLock.acquire();
        if (!entry.isReadOnly() && entry.isDirty() && entry.isValid()) {
            int offset = getOffsetOfPage(entry.getPid(), entry.getVpn());
            if (offset < 0)
                offset = getFreeOffset();
            
            int pageSize = Processor.pageSize;
            byte[] memory = Machine.processor().getMemory();
            int paddr = Processor.makeAddress(entry.getPpn(), 0);
            int totalWrite = swapFile.write(offset*pageSize, memory, paddr, pageSize);
            if (totalWrite != pageSize) {
                freeList.add(offset);
                swapLock.release();
                return false;
            }
            addOffsetOfPage(entry.getPid(), entry.getVpn(), offset);
            TestStubs.assertTrue(totalWrite == pageSize);    
        }
        
      //Invalidate memory
        swapOutCount++;
        MemoryMap mm = MemoryMap.getMemoryMap();
        mm.removeFromMemory(entry);
        
        swapLock.release();
        return true;
    }


    /**
     * Moves a page from swap into physical memory.
     * Checks in the entry for it, writing it to the TLB.
     * @param entry
     * @return true on success
     */
    public boolean moveFromSwap(MemoryMapEntry entry) {
        
        TestStubs.debugPrint("MoveFromSwap " + entry.toString());
        
        if (!entry.isPinned()) {
            TestStubs.assertTrue(false);
            return false;
        }
        
        swapLock.acquire();
        int offset = getOffsetOfPage(entry.getPid(), entry.getVpn());
        if (offset < 0) {
            swapLock.release();
            return false;
        }

        int pageSize = Processor.pageSize;
        byte[] memory = Machine.processor().getMemory();
        int paddr = Processor.makeAddress(entry.getPpn(), 0);
        int totalRead = swapFile.read(offset*pageSize, memory, paddr, pageSize);
        if (totalRead != pageSize) {
            swapLock.release();
            return false;
        }
        TestStubs.assertTrue(totalRead == pageSize);
        swapInCount++;
        entry.setReadOnly(false);
        MemoryMap mm = MemoryMap.getMemoryMap();
        mm.checkIn(entry, MemoryMap.checkInEffect.WRITE_TO_TLB);
        
        swapLock.release();
        return true;
    }
    
    
    public void invalidateProcess(int pid) {
        swapLock.acquire();
        Hashtable<Integer, Integer> processTable = swapFileTable.remove(pid);
        if (processTable != null)       
            for (Integer pos : processTable.values()) {
                freeList.add(pos);
            }
        swapLock.release();
    }
    
    /**
     * Returns either a known offset for this page, or a blank one.
     * @param pid
     * @param vpn
     * @return
     */
    private int getFreeOffset() {
        if (!freeList.isEmpty()) {
            return freeList.removeFirst();
        }
        swapFileSize++;
        return swapFileSize-1;
    }
    private int getOffsetOfPage(int pid, int vpn) {
        Hashtable<Integer, Integer> processTable = swapFileTable.get(pid);
        if (processTable == null)
            return -1;
        Integer offset = processTable.get(vpn);
        if (offset == null)
            return -1;
        return offset;
    }
    
    private void addOffsetOfPage(int pid, int vpn, int offset) {
        Hashtable<Integer, Integer> processTable = swapFileTable.get(pid);
        if (processTable == null) {
            processTable = new Hashtable<Integer, Integer>();
            swapFileTable.put(pid, processTable);
        }
        Integer currOffset = processTable.get(vpn);
        if (currOffset == null)
            processTable.put(vpn, offset);
        else
            TestStubs.assertTrue(currOffset == offset);
        
    }

    private Swap() {
        swapLock = new Lock();
        swapFile = ThreadedKernel.fileSystem.open(swapFileName, true);
        swapFileSize = 0;
        freeList = new LinkedList<Integer>();
        swapFileTable = new Hashtable<Integer, Hashtable<Integer,Integer>>();
    }
    
    public static Swap getSwap() {
        boolean status = Machine.interrupt().disable();
        if (theOnlySwap == null)
            theOnlySwap = new Swap();
        Machine.interrupt().restore(status);
        return theOnlySwap;
    }
    
    private static Swap theOnlySwap;
    
    
    
    /*
     * 
     * TEST CODE FOLLOWS
     * 
     * 
     */
    
    
    public static void selfTest() {
        
        TestStubs.testStartPrint("TESTING SWAP STARTS!");
        test1();
        TestStubs.testDonePrint("TESTING SWAP DONE!");
    }

    private static void test1() {
        
        MemoryMap.getMemoryMap().close();
        MemoryMap mm = MemoryMap.getMemoryMap();
        
        TestStubs.debugPrint("Getting a free page, setting it, and checking it back.");
        MemoryMapEntry entry1 = mm.checkOut(1);
        TestStubs.assertTrue(!entry1.isValid());
        entry1.setPid(1);
        entry1.setVpn(450);
        entry1.setReadOnly(false);
        entry1.setDirty();
        mm.checkIn(entry1, MemoryMap.checkInEffect.WRITE_TO_TLB);
        TestStubs.printTLBContents();
        TestStubs.debugPrint(entry1.toString());
        
        MemoryMapEntry entry2 = mm.checkOut(1, 450);
        TestStubs.debugPrint(entry2.toString());
        Swap.getSwap().moveToSwap(entry2);
        TestStubs.debugPrint(entry2.toString());
        TestStubs.debugPrint("Is it in swap? " + Swap.getSwap().contains(1, 450));
        
        
        
        
    }

    public static int getSwapInCount() {
        return swapInCount;
    }

    public static int getSwapOutCount() {
        return swapOutCount;
    }

    public static int getSwapFileSize() {
        return lastSwapFileSize;
    }
}
