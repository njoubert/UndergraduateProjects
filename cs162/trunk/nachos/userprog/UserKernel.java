package nachos.userprog;

import nachos.machine.*;
import nachos.threads.*;
import nachos.vm.VMKernel;

import java.util.LinkedList;

/**
 * A kernel that can support multiple user processes.
 */
public class UserKernel extends ThreadedKernel {
    /**
     * Maintain a global linked list of free physical pages.
     */
	protected static LinkedList<Integer> freeList;
	protected static Lock freeListLock;
		
	/**
     * Allocate a new user kernel.
     */
    public UserKernel() {
    	super();
    	freeList = new LinkedList<Integer>();
    	int numPhysPages = Machine.processor().getNumPhysPages();
    	for (int i = 0; i < numPhysPages; i++)
    		freeList.addLast(i);
    }
    
    /**
     * Retrieve a physical page from freeList, if available
     */
    protected static int getNextAvailPhysPage(){
    	freeListLock.acquire();
    	int ppn = -1;
    	if(!freeList.isEmpty())
    		ppn = (int) freeList.removeFirst();
    	freeListLock.release();
    	return ppn;
    }
    
    /**
     * Return a physical page to freeList
     */
    protected static void addAvailPhysPage(int ppn){
    	if (ppn < 0 || ppn > Machine.processor().getNumPhysPages())
    		return;
    	freeListLock.acquire();
    	if(!freeList.contains(ppn))
    		freeList.addLast(ppn);
    	freeListLock.release();
    }
    
    /**
     * Initialize this kernel. Creates a synchronized console and sets the
     * processor's exception handler.
     */
    public void initialize(String[] args) {
	super.initialize(args);

	console = new SynchConsole(Machine.console());
	freeListLock = new Lock();
	
	Machine.processor().setExceptionHandler(new Runnable() {
		public void run() { exceptionHandler(); }
	    });
    }

    /**
     * Test the console device.
     */	
    public void selfTest() {
        
	super.selfTest();
	
	/*
	TestStubs.debugPrint("Testing the console device. Typed characters");
	TestStubs.debugPrint("will be echoed until q is typed.");

	char c;

	do {
	    c = (char) console.readByte(true);
	    console.writeByte(c);
	}
	while (c != 'q');

	TestStubs.debugPrint("");
	
	//Our UserProcess selfTest code:
	UserProcess.selfTest();
       */
    }

    /**
     * Returns the current process.
     *
     * @return	the current process, or <tt>null</tt> if no process is current.
     */
    public static UserProcess currentProcess() {
	if (!(KThread.currentThread() instanceof UThread))
	    return null;
	
	return ((UThread) KThread.currentThread()).process;
    }

    /**
     * The exception handler. This handler is called by the processor whenever
     * a user instruction causes a processor exception.
     *
     * <p>
     * When the exception handler is invoked, interrupts are enabled, and the
     * processor's cause register contains an integer identifying the cause of
     * the exception (see the <tt>exceptionZZZ</tt> constants in the
     * <tt>Processor</tt> class). If the exception involves a bad virtual
     * address (e.g. page fault, TLB miss, read-only, bus error, or address
     * error), the processor's BadVAddr register identifies the virtual address
     * that caused the exception.
     */
    public void exceptionHandler() {
	Lib.assertTrue(KThread.currentThread() instanceof UThread);

	UserProcess process = ((UThread) KThread.currentThread()).process;
	int cause = Machine.processor().readRegister(Processor.regCause);
	process.handleException(cause);
    }

    /**
     * Start running user programs, by creating a process and running a shell
     * program in it. The name of the shell program it must run is returned by
     * <tt>Machine.getShellProgramName()</tt>.
     *
     * @see	nachos.machine.Machine#getShellProgramName
     */
    public void run() {
	super.run();
	
	UserProcess process = UserProcess.newUserProcess();
	
	String shellProgram = Machine.getShellProgramName();	
	Lib.assertTrue(process.execute(shellProgram, new String[] { }));

	KThread.currentThread().finish();
    }

    /**
     * Terminate this kernel. Never returns.
     */
    public void terminate() {
	super.terminate();
    }

    /** Globally accessible reference to the synchronized console. */
    public static SynchConsole console;

    // dummy variables to make javac smarter
    private static Coff dummy1 = null;
}
