/**
 * 
 */
package nachos.vm;

import java.util.Hashtable;
import java.util.Random;
import nachos.machine.Machine;
import nachos.machine.TranslationEntry;
import nachos.threads.Condition2;
import nachos.threads.Lock;
import nachos.threads.TestStubs;

/**
 * A mapping from (PID,VPN) to PPN and vice versa using MemoryMapEntries.
 * Allows for constant-time lookups of both PID,VPN and PPN keys to MemoryMapEntries.
 * Thread-Safe on a MemoryMapEntry level.
 * 
 * THIS CLASS IS THE ONLY PLACE WHERE WE TOUCH: <BR/>
 * - TLB <BR/>
 * - Inverted Page Table <BR/>
 * - Core Map <BR/>
 * <BR/>
 * This class follows the Consumer/Producer scheme, with checking out pages (and pinning them)
 * being equivalent to consuming, and checking in pages (and unpinning them) being equivalent
 * to producing.
 * 
 * @author Niels Joubert, Nadeem Laiwala
 *
 */
public class MemoryMap {
    
    private Hashtable<Integer, Hashtable<Integer, Integer> > invertedPageTable;
    private MemoryMapEntry[] coreMap;
    private int clockHand;              //for finding free or replaceable pages with clockalgorithm
    private int pageFaultCount;
    /* HERE STARTS THE MONITOR DATASTRUCTURES */
    private int numUnpinnedPages;       //the number of unpinned pages.
    private Lock mmLock;                        //provides synchronicity.
    private Condition2 pagesAvailable;          //sleep on this if there are no pages unpinned.
    /* HERE ENDS THE MONITOR DATASTRUCTURES */
    
    private Random generator;
    
    /**
     * Creates a MemoryMap
     * Initially EMPTY inverted page table
     * coreMap with all invalid, unpinned entries.
     */
    @SuppressWarnings("synthetic-access")
    private MemoryMap() {
            //Instantiate Monitor code for producer/consumer code
        mmLock = new Lock();
        pagesAvailable = new Condition2(mmLock);
        generator = new Random();
        numUnpinnedPages = Machine.processor().getNumPhysPages();
        clockHand = 0;
        invertedPageTable = new Hashtable<Integer, Hashtable<Integer, Integer> >();
        coreMap = new MemoryMapEntry[numUnpinnedPages];
        for (int ppn = 0; ppn < numUnpinnedPages; ppn++) {
            coreMap[ppn] = new MemoryMapEntry(ppn);
        }
        
    }
    
    
    /**
     * Controls the behavior of checking in an entry with respect to the TLB. <br/>
     *  - Writing entry to TLB: Pagefaults successfully loaded, updating TLB after TLB miss.<br/>
     *  - Invalidate if in TLB: context switches<br/>
     *  - Ignoring current TLB: reading and writing virtual memory.<br/>
     * @author Niels Joubert, Nadeem Laiwala
     */
    enum checkInEffect { WRITE_TO_TLB, INVALIDATE_IF_IN_TLB, IGNORE_TLB; }
    
    /**
     * Checks out the MME corresponding to the given pid, vpn.
     * THIS MUST BE A VALID PAGE!
     * Use checkOut() with no arguments to get a free page.
     * Returns null if:<br/>
     * - process not in IPT<br/>
     * - vpn not valid in IPT<br/>
     * - page is pinned.<br/>
     * If this page is already pinned, you can't get it.
     * @param pid
     * @param vpn
     * @return A Valid, Pinned page referring to real data. (Thus you cannot change VPN/PID)
     */
    @SuppressWarnings("synthetic-access")
    public MemoryMapEntry checkOut(int pid, int vpn) {
        //TLBtoMemoryMap(pid, false);
        mmLock.acquire();
        Hashtable<Integer, Integer> processTable = invertedPageTable.get(pid);
        
        if (processTable == null) {
            mmLock.release();
            return null;
        }
        
        Integer ppn = processTable.get(vpn);
        if (ppn == null) {
            mmLock.release();
            return null;
        }
        
        if (coreMap[ppn].isPinned() || !coreMap[ppn].isValid()) {
            mmLock.release();
            return null;
        }
        
        coreMap[ppn].pinThis();         //have to pin the entry in the coremap before copying!
        
        MemoryMapEntry retval = new MemoryMapEntry(coreMap[ppn]);
        
        TestStubs.testStartPrint("CHECKOUT (PID=" + pid + "): RETURNING PPN " + retval.getPpn() + ", " + numUnpinnedPages  + " unpinned left, Checking out entry " + retval.toString());
        
        mmLock.release();
        return retval;
        
    }
        
    /**
     * Checkout any page for you to use.<BR/>
     * returns VALID page IF - You have to Swap it (in MemoryDriver)<BR/>
     * returns INVALID page IF - you can use it right off the bat!<BR/>
     * 
     */
    @SuppressWarnings("synthetic-access")
    public MemoryMapEntry checkOut(int pid) {
        TLBtoMemoryMap(pid, false);
        mmLock.acquire();
        
        //TestStubs.debugPrint("UNPINNED: " + numUnpinnedPages + " Checking out CLOCKALGORITHM");
        
        //if no pages, sleep on cond var.
        while (numUnpinnedPages < 1) {
            pagesAvailable.sleep();
        }
        
        //*
        
        //Run clock algorithm to select a page to return
        boolean done = false;
        MemoryMapEntry entry = coreMap[clockHand];
        while (!done) {
            entry = coreMap[clockHand];
            if (entry.isPinned()) {
                //move on
            } else if (!entry.isValid()) {
                done = true;            //This one is free for the picking
                entry.pinThis();
            } else if (entry.isUsed())
                entry.setUsed(false);
            else {
                done = true;
                entry.pinThis();        //Have to swap this one out
            }
            clockHand++;
            clockHand = clockHand % Machine.processor().getNumPhysPages();
        }
        // */
        /*
        boolean done = false;
        int n = generator.nextInt(Machine.processor().getNumPhysPages());
        MemoryMapEntry entry = coreMap[n];
        while (!done) {
            entry = coreMap[n];
            n = generator.nextInt(Machine.processor().getNumPhysPages());
            if (entry.isPinned()) {
                //Nothing doing
            } else {
                entry.pinThis();
                done = true;
            }
            
        }
        // */
        
        if (entry.vpn == 0)
            TestStubs.debugPrint("stalling");
        TestStubs.assertTrue(entry.isPinned());
        
        //if this page happens to be in the TLB, invalidate it
        TranslationEntry e;
        for (int i = 0; i < Machine.processor().getTLBSize(); i++) {
            e = Machine.processor().readTLBEntry(i);
            if (e.ppn == entry.ppn && e.valid && entry.isValid())
                TLBEntryToMemoryMap(pid, true, i, false);
        }
        
        MemoryMapEntry retval = new MemoryMapEntry(coreMap[entry.ppn]);
        TestStubs.testStartPrint("CHECKOUT (PID=" + pid + "): RETURNING PPN " + retval.getPpn() + ", " + numUnpinnedPages  + " unpinned left, CLOCKAGL Checking out entry " + retval.toString());
        
        mmLock.release();
        return retval;
        
    }
    
    /**
     * Checks in the given entry into the MemoryMap.<br/>
     * The PID and VPN *MUST* be set correctly.<br/>
     * 
     * ONLY TAKES IN VALID PAGES! IT IS UP TO YOU TO VALIDATE THIS ENTRY! 
     * @param entry
     * @param effect
     * @return
     */
    @SuppressWarnings("synthetic-access")
    public boolean checkIn(MemoryMapEntry entry, checkInEffect effect) {
        TestStubs.testDonePrint("CHECKIN: RECEIVING PPN "+entry.getPpn() + ", " + numUnpinnedPages  + " unpinned left, Checking IN entry " + entry.toString());
        
        mmLock.acquire();
        TestStubs.assertTrue(entry.isPinned());
        entry.setUsed(true);
        
        Hashtable<Integer, Integer> processTable = invertedPageTable.get(entry.pid);
        if (entry.isValid()) {  //If this page is valid, it must already by in the IPT
            
            if (processTable == null) {
                mmLock.release();
                TestStubs.assertTrue(false); //FUCK THIS SHOULD NOT BE HAPPENING
                return false; //OHMY GOSH IM NOT GOOD WITH COMPUTERS HOW DID THIS HAPPEN?
            }
            Integer ppn = processTable.get(entry.vpn);
            if (ppn == null) {
                mmLock.release();
                TestStubs.assertTrue(false);
                return false;
            }
            
        } else {                //If this entry is not valid, it must be inserted into IPT.
            
            TestStubs.assertTrue(entry.getPid() >= 0);
            TestStubs.assertTrue(entry.getVpn() >= 0);
            
            if (processTable == null) {
                processTable = new Hashtable<Integer, Integer>();
                invertedPageTable.put(entry.pid, processTable);
            }
            Integer oldValue = processTable.put(entry.vpn, entry.ppn); //NAdeem skips my asserts :-(
            TestStubs.assertTrue(oldValue == null); //sad fo youuuuuuuuuuuuuuuuuuuuuuuuuuu
            entry.setValid(true);
        }
        
        //AT THIS POINT WE HAVE AN ENTRY IN THE IPT FOR THE GIVEN MME

        //Handling the TLB
        switch(effect) {
        case WRITE_TO_TLB:              //For handling pagefaults
            writeEntryToTLB(entry);
            break;
        case INVALIDATE_IF_IN_TLB:      //For context switching and other bullshit
            invalidateTLBEntry(entry.ppn);
            break;
        case IGNORE_TLB:                //R/W VM that should not be modifying TLB
            break;
        }
        
        entry.unPinThis();
        
        coreMap[entry.ppn] = new MemoryMapEntry(entry);
        pagesAvailable.wake();          //Notify waiting threads that a page has been checked in
        System.out.flush();
        mmLock.release();
        return true;
    }

    /**
     * Moving a page from memory to swap<BR/>
     * Removes the entries for this page from our MemoryMap.
     * @param entry that is valid that has been moved to SWAP.
     * @return SETS THIS ENTRY TO BE INVALID, returns true if succeeds.
     */
    @SuppressWarnings("synthetic-access")
    public boolean removeFromMemory(MemoryMapEntry entry) {
        TestStubs.debugPrint("removeFromMemory: " + entry.toString());
        mmLock.acquire();
        TestStubs.assertTrue(entry.isPinned());
        
        if (entry.isValid()) {  //If this page is valid, REMOVE!
            invalidateTLBEntry(entry.ppn);
            Hashtable<Integer, Integer> processTable = invertedPageTable.get(entry.pid);
            if (processTable == null) {
                mmLock.release();
                TestStubs.assertTrue(false);
                return false; //WTF how come we dont have it?!
            }
            
            Integer ppn = processTable.remove(entry.vpn);
            TestStubs.assertTrue(ppn.equals(entry.ppn));
            
            entry.setReadOnly(false);
            entry.setValid(false); //Invalidate that bitch.
            coreMap[ppn].setReadOnly(false);
            coreMap[ppn].setValid(false);
            mmLock.release();
            return true;
        }
        
        mmLock.release();
        return true; //WHAT THE FUCK WERE YOU THINKING!? PASSING MY AN INVALID ENTRY?
    }
    
    /**
     * This function obliterates everything a process has done and frees his memory.
     * This is for calling when the process exits.
     * @param pid - the pid of the process to destroy.
     */
    @SuppressWarnings("synthetic-access")
    public void invalidateProcess(int pid) {
        mmLock.acquire();
        TLBtoMemoryMap(pid, true);
       
        Hashtable<Integer, Integer> processTable = invertedPageTable.remove(pid);
        if (processTable == null) {
            mmLock.release();
            return;
        }
        
        for (Integer n : processTable.values()) {
            coreMap[n] = new MemoryMapEntry(n);
        }
        
        mmLock.release();
    }
    
    /**
     * Performs write-back from your TLB to your MemoryMap.
     * mmLock should NOT be locked. This is Atomic by disabling interrupts.
     */
    public void TLBtoMemoryMap(int pid, boolean invalidateEntries) {
        boolean status = Machine.interrupt().disable();
        for (int t = 0; t < Machine.processor().getTLBSize(); t++) {
            TLBEntryToMemoryMap(pid, invalidateEntries, t, true);
        }
        Machine.interrupt().restore(status);
    }
    
    @SuppressWarnings("synthetic-access")
    public void touchPhysicalPage(int ppn, boolean makeDirty) {
        if (ppn < 0 || ppn >= Machine.processor().getNumPhysPages())
            return;
        TestStubs.assertTrue(!coreMap[ppn].isPinned());
        coreMap[ppn].setUsed(true);
        if (makeDirty)
            coreMap[ppn].setDirty();
    }
    
    /**
     * Performs a write-back on a single entry in your TLB.
     * @param pid
     * @param invalidateEntries
     * @param n
     */
    @SuppressWarnings("synthetic-access")
    private void TLBEntryToMemoryMap(int pid, boolean invalidateEntries, int n, boolean careAboutPins) {
        boolean status = Machine.interrupt().disable();
        TranslationEntry tlbEntry = Machine.processor().readTLBEntry(n);
        if (tlbEntry.dirty && tlbEntry.valid) {
            
            TestStubs.assertTrue(coreMap[tlbEntry.ppn].ppn == tlbEntry.ppn);
            TestStubs.assertTrue(coreMap[tlbEntry.ppn].vpn == tlbEntry.vpn);
            
            if (coreMap[tlbEntry.ppn].isPinned() && careAboutPins) {
                if (!invalidateEntries) {
                    TestStubs.assertTrue(coreMap[tlbEntry.ppn].dirty == tlbEntry.dirty);
                    TestStubs.assertTrue(coreMap[tlbEntry.ppn].valid == tlbEntry.valid);
                    TestStubs.assertTrue(coreMap[tlbEntry.ppn].readOnly == tlbEntry.readOnly);
                    TestStubs.assertTrue(coreMap[tlbEntry.ppn].used == tlbEntry.used);
                    TestStubs.assertTrue(coreMap[tlbEntry.ppn].vpn == tlbEntry.vpn);
                    TestStubs.assertTrue(coreMap[tlbEntry.ppn].pid == pid);
                } else {
                    TestStubs.assertTrue(false);
                }
            } else {
                coreMap[tlbEntry.ppn].dirty = tlbEntry.dirty;
                coreMap[tlbEntry.ppn].valid = tlbEntry.valid;
                coreMap[tlbEntry.ppn].readOnly = tlbEntry.readOnly;
                coreMap[tlbEntry.ppn].used = tlbEntry.used;
                coreMap[tlbEntry.ppn].vpn = tlbEntry.vpn;
                coreMap[tlbEntry.ppn].pid = pid;
            }
            
        }
        if (invalidateEntries)
            Machine.processor().writeTLBEntry(n, new TranslationEntry());
        Machine.interrupt().restore(status);
    }
    
    /**
     * Writes a TLB entry for the given MME. MME must be pinned.
     * @param entry
     */
    @SuppressWarnings("synthetic-access")
    private void writeEntryToTLB(MemoryMapEntry entry) {

        TestStubs.assertTrue(entry.isPinned());
        
        
        
        //Get the entry to replace
        int n = generator.nextInt(Machine.processor().getTLBSize());
        for (int i = 0; i < Machine.processor().getTLBSize(); i++)
            if (!(Machine.processor().readTLBEntry(i).valid)) {
                n = i;
                break;
            }
                
        //Write that once back to memory
        TLBEntryToMemoryMap(entry.pid, true, n, true);
        
        //now write my entry to it
        TranslationEntry newTLBentry = new TranslationEntry();
        newTLBentry.ppn = entry.ppn;
        newTLBentry.vpn = entry.vpn;
        newTLBentry.valid = entry.valid;
        newTLBentry.readOnly = entry.readOnly;
        newTLBentry.dirty = entry.dirty;
        newTLBentry.used = entry.used;
        
        TestStubs.debugPrint("Writing entry to TLB: vpn:"+newTLBentry.vpn+" -> ppn:"+newTLBentry.ppn);
        Machine.processor().writeTLBEntry(n, newTLBentry);

    }
         

    
    /**
     * Invalidates any and all TLB entries pointing to this PPN.
     * There could be multiple ones if they're all set to 0, the default value.
     * @param ppn
     */
    private void invalidateTLBEntry(int ppn) {
        boolean status = Machine.interrupt().disable();
        for (int t = 0; t < Machine.processor().getTLBSize(); t++) {
            TranslationEntry tlbEntry = Machine.processor().readTLBEntry(t);
            if (ppn == tlbEntry.ppn) {
                Machine.processor().writeTLBEntry(t, new TranslationEntry());
            }
        }
        Machine.interrupt().restore(status);
    }

    /**
     * Represents an entry stored in our MemoryMap.
     * @author Niels Joubert, Nadeem Laiwala
     */
    public class MemoryMapEntry {

        private int pid;
        private int vpn;
        private int ppn; //THIS SHOULD NEVER BE SET BY ANYONE OUTSIDE MEMORYMAP
        
        private boolean dirty;
        private boolean used;
        private boolean valid;
        private boolean readOnly;
        
        private boolean pinned;
        
        @Override
        public String toString() {
            return "MemoryMapEntry [pid="+pid+",vpn="+vpn+"->ppn="+ppn+"] " +
            		"pinned:"+pinned +
            		" flags:" +
            		"(dirty="+dirty+",used="+used+",valid="+valid+",readOnly="+readOnly+")";
        }
        
        /**
         * Copies the given entry.
         * @param entry
         */
        MemoryMapEntry(MemoryMapEntry entry) {
            this.pid = entry.pid;
            this.vpn = entry.vpn;
            this.ppn = entry.ppn;
            this.dirty = entry.dirty;
            this.used = entry.used;
            this.valid = entry.valid;
            this.readOnly = entry.readOnly;
            this.pinned = entry.pinned;
        }

        /**
         * SHOULD ONLY BE CALLED INSIDE MEMORYMAP!
         * Creates the default entry for the given ppn.
         * @param ppn
         */
        private MemoryMapEntry(int ppn) {
            this.ppn = ppn;
            this.pid = -1;
            this.vpn = -1;
            this.dirty = false;
            this.used = false;
            this.valid = false;
            this.readOnly = false;
            this.pinned = false;
        }

        public boolean isPinned() {
            return pinned;
        }

        @SuppressWarnings("synthetic-access")
        private void pinThis() {
            TestStubs.assertTrue(numUnpinnedPages > 0);
            TestStubs.assertTrue(!isPinned());
            this.pinned = true;
            numUnpinnedPages--;
            
        }
        @SuppressWarnings("synthetic-access")
        private void unPinThis() {
            TestStubs.assertTrue(numUnpinnedPages >= 0);
            TestStubs.assertTrue(isPinned());
            numUnpinnedPages++;
            this.pinned = false;
        }
        
        public int getPid() {
            return pid;
        }

        public boolean setPid(int pid) { //You can't change shit if this is a valid entry.
            if (!isValid() && isPinned()) {
                this.pid = pid;
                return true;
            }
            TestStubs.assertTrue(false);
            return false;
        }

        public int getVpn() {
            return vpn;
        }

        public boolean setVpn(int vpn) {
            if (!isValid() && isPinned()) {
                this.vpn = vpn;
                return true;
            }
            TestStubs.assertTrue(false);
            return false;
        }
        
        public boolean isValid() {
            return valid;
        }
        private void setValid(boolean valid) {
           this.valid = valid;
        }

        public boolean isDirty() {
            return dirty;
        }

        public void setDirty() {
            if (!isReadOnly())
                this.dirty = true;
            else
                TestStubs.assertTrue(false);
        }

        public boolean isReadOnly() {
            return readOnly;
        }

        public void setReadOnly(boolean readOnly) {
            this.readOnly = readOnly;
        }

        @SuppressWarnings("unused")
        private boolean isUsed() {
            return used;
        }

        @SuppressWarnings("unused")
        private void setUsed(boolean used) {
            this.used = used;
        }

        public int getPpn() {
            return ppn;
        }
        
    }
    
    public void close() {
        theOnlyMemoryMap = null;
    }

    /**
     * We use the Singleton design pattern to ensure that we only have one MemoryMap
     * at any point in time, but we can easily get a reference to this MemoryMap
     * by using this function.<BR/>
     * This function is thread safe - only one MemoryMap will ever be created.
     * @return A reference to the only MemoryMap that is allowed to exist.
     * 
     */
    public static MemoryMap getMemoryMap() {
        boolean status = Machine.interrupt().disable();
        if (theOnlyMemoryMap == null)
            theOnlyMemoryMap = new MemoryMap();
        Machine.interrupt().restore(status);
        return theOnlyMemoryMap;
    }
    private static MemoryMap theOnlyMemoryMap = null;
    
    
   
    /*
     * 
     * 
     * TEST CODE FOLLOWS
     * 
     * 
     */
    
    
    public static void selfTest() {
        
        TestStubs.testStartPrint("selfTesting MEMORYMAP!");
        
        test1();
        
        TestStubs.testDonePrint("");
        
    }
    
    private static void test1() {
        MemoryMap mm = getMemoryMap();
        MemoryMapEntry me1 = mm.checkOut(0);
        MemoryMapEntry me2 = mm.checkOut(0);
        MemoryMapEntry me3 = mm.checkOut(0);
        
        TestStubs.debugPrint("Read the following as 3 consequtive checkout(0) calls:");
        TestStubs.debugPrint(me1.toString());
        TestStubs.debugPrint(me2.toString());
        TestStubs.debugPrint(me3.toString());
        
        
        me1.setPid(11);
        
        me1.setDirty();
        me1.setReadOnly(true);
        me1.setVpn(987);
        TestStubs.debugPrint("Setting Values, then checking in:");
        TestStubs.debugPrint(me1.toString());
        mm.checkIn(me1, checkInEffect.IGNORE_TLB);
        TestStubs.debugPrint(me1.toString());
        
        MemoryMapEntry me1_repeat = mm.checkOut(11, 987);
        TestStubs.assertTrue(me1_repeat.getPid() == 11);
        TestStubs.assertTrue(me1_repeat.getVpn() == 987);
        TestStubs.assertTrue(me1_repeat.isDirty());
        TestStubs.assertTrue(me1_repeat.isReadOnly());
        TestStubs.assertTrue(me1_repeat.isPinned());
        TestStubs.assertTrue(me1_repeat.isValid());
        TestStubs.debugPrint("Pulled out with checkout(11,987): ");
        TestStubs.debugPrint(me1_repeat.toString());
        mm.checkIn(me1_repeat, checkInEffect.INVALIDATE_IF_IN_TLB);
        
        TestStubs.debugPrint("Getting another with checkout(0), setting values, checking back: ");
        MemoryMapEntry me5 = mm.checkOut(0);
        TestStubs.debugPrint(me5.toString());
        me5.setPid(12);
        me5.setVpn(777);
        mm.checkIn(me5, checkInEffect.WRITE_TO_TLB);
        TestStubs.debugPrint(me5.toString());
        TestStubs.debugPrint("Also, should now have it in TLB:");
        TestStubs.printTLBContents();
        
        TestStubs.debugPrint("Invalidating process 12:");
        mm.invalidateProcess(12);
        TestStubs.printTLBContents();
        MemoryMapEntry me5_r = mm.checkOut(12, 777);
        TestStubs.debugPrint("Getting this entry now returns null: " + (me5_r == null));
        
        
    }

    public int getNumOfUnpinnedPages() {
        return numUnpinnedPages;
    }
    
    @SuppressWarnings("synthetic-access")
    public void TLBwriteback(int pid) {
        for (int i = 0; i < Machine.processor().getTLBSize(); i++) {
            TranslationEntry te = Machine.processor().readTLBEntry(i);
            
            if (te.dirty && te.valid) {
                coreMap[te.ppn].pid = pid;
                coreMap[te.ppn].vpn = te.vpn;
                coreMap[te.ppn].dirty = te.dirty;
                coreMap[te.ppn].readOnly = te.readOnly;
                coreMap[te.ppn].used = te.used;
                coreMap[te.ppn].valid = te.valid;
                TestStubs.debugPrint("Wrote back dirty entry for ppn " + te.ppn);
            }
            
        }
    }

    public int getPageFaultCount() {
        return pageFaultCount;
    }

    public void countPageFaultCount() {
        pageFaultCount++;
    }
}
