package nachos.vm;

import nachos.machine.CoffSection;
import nachos.machine.Lib;
import nachos.machine.Machine;
import nachos.machine.Processor;
import nachos.threads.Lock;
import nachos.threads.TestStubs;
import nachos.userprog.UserProcess;
import nachos.vm.MemoryMap.MemoryMapEntry;
import nachos.vm.MemoryMap.checkInEffect;

/**
 * A <tt>UserProcess</tt> that supports demand-paging.
 */
public class VMProcess extends UserProcess {
    /**
     * Allocate a new process.
     */
    public VMProcess() {
        super();
    }

    @Override
    public int readVirtualMemory(int vaddr, byte[] data, int offset, int length) {
        if ((vaddr < 0) || (data == null) || (offset < 0) || (length < 0) || (offset+length > data.length))
            return 0;
        byte[] memory = Machine.processor().getMemory();
        int vpn = Processor.pageFromAddress(vaddr), off = Processor.offsetFromAddress(vaddr), ppn = -1;
        TestStubs.debugPrint("Reading virtual memory " + vpn);
        int amount = 0, copyAmount = 0;
        while (length > 0){
            MemoryMapEntry me = resolveMemory(this, vpn, MemoryMap.checkInEffect.IGNORE_TLB);
            if (me == null)
                return 0;
            ppn = me.getPpn();
            MemoryMap.getMemoryMap().touchPhysicalPage(ppn, false);
            int paddr = Processor.makeAddress(ppn, off);
            copyAmount = Math.min(pageSize - off, length);
            System.arraycopy(memory, paddr, data, offset + amount, copyAmount);

            off = 0;
            amount += copyAmount;
            length -= copyAmount;
            vpn++;
            
        }
        return amount;
    }

    @Override
    public int writeVirtualMemory(int vaddr, byte[] data, int offset, int length) {
        if ((vaddr < 0) || (data == null) || (offset < 0) || (length < 0) || (offset+length > data.length))
            return 0;
        byte[] memory = Machine.processor().getMemory();
        int vpn = Processor.pageFromAddress(vaddr), off = Processor.offsetFromAddress(vaddr), ppn = -1;
        TestStubs.debugPrint("Writing virtual memory " + vpn);
        int amount = 0, copyAmount = 0;
        while (length > 0){ //should we possibly be able to return 0 if we somehow fail??
            MemoryMapEntry me = resolveMemory(this, vpn, MemoryMap.checkInEffect.IGNORE_TLB);
            
            if (me == null)
                return 0;
            if (me.isReadOnly())
                return 0;
            ppn = me.getPpn();
            MemoryMap.getMemoryMap().touchPhysicalPage(ppn, true);
            int paddr = Processor.makeAddress(ppn, off);
            copyAmount = Math.min(pageSize - off, length);
            
            System.arraycopy(data, offset + amount, memory, paddr, copyAmount);
            off = 0;
            amount += copyAmount;
            length -= copyAmount;
            vpn++;
        }
        return amount;
    }

    /**
     * Save the state of this process in preparation for a context switch.
     * Called by <tt>UThread.saveState()</tt>.
     */
    @Override
    public void saveState() {
        TestStubs.debugPrint("CONTEXT SWITCH COMING, SAVING STATE FOR PID " + this.pid);
	handleContextSwitch(this.pid);
    }
    
    /**
     * Restore the state of this process after a context switch. Called by
     * <tt>UThread.restoreState()</tt>.
     */
    @Override
    public void restoreState() {
    	//SHOULD BE EMPTY
        TestStubs.debugPrint("CONTEXT SWITCH DONE, RESTORED STATE FOR PID " + this.pid);
    }
    
    @Override
    protected boolean loadSections() {
	//THIS SHOULD BE EMPTY
	//THE FIRST ARGUMENT WRITE WILL CAUSE THE ARG PAGE TO BE LOADED (UserProcess.load()
	//THE REST IS LAZILY LOADED
        return true;
    }


    /**
     * Release any resources allocated by <tt>loadSections()</tt>.
     */
    @Override
    protected void unloadSections() {
        //WE DO NEED TO DO CLEANUP
        handleUnloading(this.pid);
    }    

    /**
     * Handle a user exception. Called by
     * <tt>UserKernel.exceptionHandler()</tt>. The
     * <i>cause</i> argument identifies which exception occurred; see the
     * <tt>Processor.exceptionZZZ</tt> constants.
     *
     * @param	cause	the user exception that occurred.
     */
    @Override
    public void handleException(int cause) {
        Processor processor = Machine.processor();
       
        switch (cause) {
        case Processor.exceptionTLBMiss:
       		int vaddr = processor.readRegister(Processor.regBadVAddr);
        	int vpn = Processor.pageFromAddress(vaddr);
		resolveMemory(this, vpn, MemoryMap.checkInEffect.WRITE_TO_TLB);
            break;
        default:
            super.handleException(cause);
        break;
        }
        
    }
    

        
        @SuppressWarnings("synthetic-access")
        public MemoryMapEntry resolveMemory(VMProcess proc, int vpn, checkInEffect effect) {
            
            boolean suicide = false;
            MemoryMap memmap = MemoryMap.getMemoryMap();
            Swap swap = Swap.getSwap();
            MemoryMapEntry entry;
            
            TestStubs.testStartPrint("RESOLVER START PID:"+proc.pid+" VPN:"+vpn+" Unpinned Pages: "+ memmap.getNumOfUnpinnedPages()+" Next PC: " + Machine.processor().readRegister(Processor.regNextPC) + " ==============");
            
            //memmap.TLBwriteback(this.pid);
            
            if ((entry = memmap.checkOut(proc.pid, vpn)) != null) {
                
                TestStubs.debugPrint("Loading Already-Memory-Resident entry into TLB");
                memmap.checkIn(entry, effect);
                
            } else if (swap.contains(proc.pid, vpn)) {
                
                memmap.countPageFaultCount();
                TestStubs.debugPrint("Loading a SWAP entry.");
                entry = memmap.checkOut(proc.pid);
                if (entry.isValid())
                    suicide = !swap.moveToSwap(entry);
                if (!suicide) {
                    entry.setPid(proc.pid);
                    entry.setVpn(vpn);
                    suicide = !swap.moveFromSwap(entry);
                }
                
            } else if (proc.lazyPage(vpn)) {
                
                memmap.countPageFaultCount();
                TestStubs.debugPrint("Lazy Loading.");
                entry = memmap.checkOut(proc.pid);
                if (entry.isValid())
                    suicide = !swap.moveToSwap(entry);
                entry.setPid(proc.pid);
                entry.setVpn(vpn);
                proc.lazilyLoad(entry);
                memmap.checkIn(entry, effect);
                
            } else {
                
                memmap.countPageFaultCount();
                TestStubs.debugPrint("WTF?! Not in memory, in swap, or in the COFF range.");
//                TestStubs.assertTrue(false);
//                suicide = true;
                
            }
                
            if (suicide) {
                Lib.debug('d', "Error occured! PROCESS SUICIDING! ");
                handleSyscall(syscallExit, 1, 0, 0, 0);
            }
            TestStubs.testDonePrint("RESOLVER DONE PID:"+proc.pid+" VPN:"+vpn+" ==============\n");
            return entry;       
        }
        
        public void handleContextSwitch(int pid) {
            MemoryMap.getMemoryMap().TLBtoMemoryMap(pid, true);
        }
        
        public void handleUnloading(int pid) {
            MemoryMap.getMemoryMap().invalidateProcess(pid);
            Swap.getSwap().invalidateProcess(pid);
        }
        

    private boolean lazyPage(int vpn) {
        if (vpn < numPages) {
            return true;
        }
        return false;
            
    }
    
    private void lazilyLoad(MemoryMapEntry entry) {
        TestStubs.debugPrint("Lazily loading a page...");
        int numCoffPages = numPages - stackPages - 1;
        boolean done = false;
        
        if (entry.getVpn() < numCoffPages) {
            
            for (int s = 0; s < coff.getNumSections(); s++) {
                CoffSection sec = coff.getSection(s);
                for (int spn = 0; spn < sec.getLength(); spn++) {
                    if ((entry.getVpn() - spn) == sec.getFirstVPN()) {
                        sec.loadPage(spn, entry.getPpn());
                        entry.setReadOnly(sec.isReadOnly());
                        done = true;
                        break;
                        
                    }
                }
                if (done) {
                    TestStubs.debugPrint("loaded a COFF page...");
                    break;
                }
            }
            
        } else {
            TestStubs.debugPrint("grew a STACK page...");
            entry.setReadOnly(false);
        }
    }

    public static int getActivePIDs() {
        return activePIDs;
    }

    private static final int pageSize = Processor.pageSize;
    @SuppressWarnings("unused")
    private static final char dbgProcess = 'a';
    @SuppressWarnings("unused")
    private static final char dbgVM = 'v';
    


}
