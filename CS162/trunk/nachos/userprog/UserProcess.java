package nachos.userprog;

import nachos.machine.*;
import nachos.threads.*;
import nachos.userprog.*;
import nachos.vm.VMKernel;

import java.io.EOFException;
import java.util.ArrayList;
import java.util.Hashtable;

/**
 * Encapsulates the state of a user process that is not contained in its
 * user thread (or threads). This includes its address translation state, a
 * file table, and information about the program being executed.
 *
 * <p>
 * This class is extended by other classes to support additional functionality
 * (such as additional syscalls).
 * 
 * @see	nachos.vm.VMProcess
 * @see	nachos.network.NetProcess
 */
public class UserProcess {
    /**
     * Allocate a new process.
     */
    public UserProcess() {
        LSLock = new Lock();
        openFileTable = new OpenFile[16];
        openFileTable[0] = UserKernel.console.openForReading(); // standard input
        openFileTable[1] = UserKernel.console.openForWriting(); // standard output
        pid = processIDcounter;
        processIDcounter++;
    }

    /**
     * Allocate and return a new process of the correct class. The class name
     * is specified by the <tt>nachos.conf</tt> key
     * <tt>Kernel.processClassName</tt>.
     *
     * @return	a new process of the correct class.
     */
    public static UserProcess newUserProcess() {
        return (UserProcess)Lib.constructObject(Machine.getProcessClassName());
    }

    /**
     * Execute the specified program with the specified arguments. Attempts to
     * load the program, and then forks a thread to run it.
     *
     * @param	name	the name of the file containing the executable.
     * @param	args	the arguments to pass to the executable.
     * @return	<tt>true</tt> if the program was successfully executed.
     */
    public boolean execute(String name, String[] args) {
        if (!load(name, args))
            return false;

        activePIDs++;
        (myThread = new UThread(this)).setName(name).fork();

        return true;
    }

    /**
     * Save the state of this process in preparation for a context switch.
     * Called by <tt>UThread.saveState()</tt>.
     */
    public void saveState() {
    }

    /**
     * Restore the state of this process after a context switch. Called by
     * <tt>UThread.restoreState()</tt>.
     */
    public void restoreState() {
        Machine.processor().setPageTable(pageTable);
    }

    /**
     * Read a null-terminated string from this process's virtual memory. Read
     * at most <tt>maxLength + 1</tt> bytes from the specified address, search
     * for the null terminator, and convert it to a <tt>java.lang.String</tt>,
     * without including the null terminator. If no null terminator is found,
     * returns <tt>null</tt>.
     *
     * @param	vaddr	the starting virtual address of the null-terminated
     *			string.
     * @param	maxLength	the maximum number of characters in the string,
     *				not including the null terminator.
     * @return	the string read, or <tt>null</tt> if no null terminator was
     *		found.
     */
    public String readVirtualMemoryString(int vaddr, int maxLength) {
        Lib.assertTrue(maxLength >= 0);

        byte[] bytes = new byte[maxLength+1];

        int bytesRead = readVirtualMemory(vaddr, bytes);

        for (int length=0; length<bytesRead; length++) {
            if (bytes[length] == 0)
                return new String(bytes, 0, length);
        }

        return null;
    }

    /**
     * Transfer data from this process's virtual memory to all of the specified
     * array. Same as <tt>readVirtualMemory(vaddr, data, 0, data.length)</tt>.
     *
     * @param	vaddr	the first byte of virtual memory to read.
     * @param	data	the array where the data will be stored.
     * @return	the number of bytes successfully transferred.
     */
    public int readVirtualMemory(int vaddr, byte[] data) {
        return readVirtualMemory(vaddr, data, 0, data.length);
    }

    /**
     * Transfer data from this process's virtual memory to the specified array.
     * This method handles address translation details. This method must
     * <i>not</i> destroy the current process if an error occurs, but instead
     * should return the number of bytes successfully copied (or zero if no
     * data could be copied).
     *
     * @param	vaddr	the first byte of virtual memory to read.
     * @param	data	the array where the data will be stored.
     * @param	offset	the first byte to write in the array.
     * @param	length	the number of bytes to transfer from virtual memory to
     *			the array.
     * @return	the number of bytes successfully transferred.
     */
    public int readVirtualMemory(int vaddr, byte[] data, int offset, int length) {
        if ((vaddr < 0) || (data == null) || (offset < 0) || (length < 0) || (offset+length > data.length))
            return 0;
        byte[] memory = Machine.processor().getMemory();
        int vpn = Processor.pageFromAddress(vaddr), off = Processor.offsetFromAddress(vaddr), ppn = -1;
        int amount = 0, copyAmount = 0;

        while (length > 0){
            if (vpn < 0 || vpn > pageTable.length)
                break;
            pageTable[vpn].used = true;
            ppn = pageTable[vpn].ppn;
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

    /**
     * Transfer all data from the specified array to this process's virtual
     * memory.
     * Same as <tt>writeVirtualMemory(vaddr, data, 0, data.length)</tt>.
     *
     * @param	vaddr	the first byte of virtual memory to write.
     * @param	data	the array containing the data to transfer.
     * @return	the number of bytes successfully transferred.
     */
    public int writeVirtualMemory(int vaddr, byte[] data) {
        return writeVirtualMemory(vaddr, data, 0, data.length);
    }

    /**
     * Transfer data from the specified array to this process's virtual memory.
     * This method handles address translation details. This method must
     * <i>not</i> destroy the current process if an error occurs, but instead
     * should return the number of bytes successfully copied (or zero if no
     * data could be copied).
     *
     * @param	vaddr	the first byte of virtual memory to write.
     * @param	data	the array containing the data to transfer.
     * @param	offset	the first byte to transfer from the array.
     * @param	length	the number of bytes to transfer from the array to
     *			virtual memory.
     * @return	the number of bytes successfully transferred.
     */
    public int writeVirtualMemory(int vaddr, byte[] data, int offset, int length) {
        if ((vaddr < 0) || (data == null) || (offset < 0) || (length < 0) || (offset+length > data.length))
            return 0;
        byte[] memory = Machine.processor().getMemory();
        int vpn = Processor.pageFromAddress(vaddr), off = Processor.offsetFromAddress(vaddr), ppn = -1;

        int amount = 0, copyAmount = 0;
        while (length > 0){
            if (vpn < 0 || vpn > pageTable.length)
                break;
            ppn = pageTable[vpn].ppn;
            if (pageTable[vpn].readOnly)
                break;
            pageTable[vpn].dirty = true;
            pageTable[vpn].used = true;
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
     * Load the executable with the specified name into this process, and
     * prepare to pass it the specified arguments. Opens the executable, reads
     * its header information, and copies sections and arguments into this
     * process's virtual memory.
     *
     * @param	name	the name of the file containing the executable.
     * @param	args	the arguments to pass to the executable.
     * @return	<tt>true</tt> if the executable was successfully loaded.
     */
    private boolean load(String name, String[] args) {
        Lib.debug(dbgProcess, "UserProcess.load(\"" + name + "\")");

        OpenFile executable = ThreadedKernel.fileSystem.open(name, false);
        if (executable == null) {
            Lib.debug(dbgProcess, "\topen failed");
            return false;
        }

        try {
            coff = new Coff(executable);
        }
        catch (EOFException e) {
            executable.close();
            Lib.debug(dbgProcess, "\tcoff load failed");
            return false;
        }

        // make sure the sections are contiguous and start at page 0
        numPages = 0;
        for (int s=0; s<coff.getNumSections(); s++) {
            CoffSection section = coff.getSection(s);
            if (section.getFirstVPN() != numPages) {
                coff.close();
                Lib.debug(dbgProcess, "\tfragmented executable");
                return false;
            }
            numPages += section.getLength();
        }

        // make sure the argv array will fit in one page
        byte[][] argv = new byte[args.length][];
        int argsSize = 0;
        for (int i=0; i<args.length; i++) {
            argv[i] = args[i].getBytes();
            // 4 bytes for argv[] pointer; then string plus one for null byte
            argsSize += 4 + argv[i].length + 1;
        }
        if (argsSize > pageSize) {
            coff.close();
            Lib.debug(dbgProcess, "\targuments too long");
            return false;
        }

        // program counter initially points at the program entry point
        initialPC = coff.getEntryPoint();	

        // next comes the stack; stack pointer initially points to top of it
        numPages += stackPages;
        initialSP = numPages*pageSize;

        // and finally reserve 1 page for arguments
        numPages++;

        if (!loadSections())
            return false;

        // store arguments in last page
        int entryOffset = (numPages-1)*pageSize;
        int stringOffset = entryOffset + args.length*4;

        this.argc = args.length;
        this.argv = entryOffset;
        
        TestStubs.debugPrint("About to write args to page vpn=" + (numPages-1));
        
        for (int i=0; i<argv.length; i++) {
            byte[] stringOffsetBytes = Lib.bytesFromInt(stringOffset);
            Lib.assertTrue(writeVirtualMemory(entryOffset,stringOffsetBytes) == 4);
            entryOffset += 4;
            Lib.assertTrue(writeVirtualMemory(stringOffset, argv[i]) == argv[i].length);
            stringOffset += argv[i].length;
            Lib.assertTrue(writeVirtualMemory(stringOffset,new byte[] { 0 }) == 1);
            stringOffset += 1;
        }

        return true;
    }

    /**
     * Allocates memory for this process, and loads the COFF sections into
     * memory. If this returns successfully, the process will definitely be
     * run (this is the last step in process initialization that can fail).
     *
     * @return	<tt>true</tt> if the sections were successfully loaded.
     */
    protected boolean loadSections() {
        LSLock.acquire();
        if (numPages > UserKernel.freeList.size()) {
            coff.close();
            Lib.debug(dbgProcess, "\tinsufficient physical memory");
            LSLock.release();
            return false;
        }
        // initialize page table
        pageTable = new TranslationEntry[numPages];

        int vpn = 0, ppn = -1;
        for (int s=0; s<coff.getNumSections(); s++) {
            CoffSection section = coff.getSection(s);
            Lib.debug(dbgProcess, "\tinitializing " + section.getName()
                    + " section (" + section.getLength() + " pages)");
            for (int i=0; i<section.getLength(); i++) {
                vpn = section.getFirstVPN()+i;
                ppn = UserKernel.getNextAvailPhysPage();
                pageTable[vpn] = new TranslationEntry(vpn,ppn,true,section.isReadOnly(),false,false);
                section.loadPage(i, ppn);
            }
        }
        for (int s=0; s<stackPages+1; s++){
            vpn++;
            ppn = UserKernel.getNextAvailPhysPage();
            pageTable[vpn] = new TranslationEntry(vpn, ppn, true, false, false, false);
        }
        LSLock.release();
        return true;
    }

    /**
     * Release any resources allocated by <tt>loadSections()</tt>.
     */
    protected void unloadSections() {
        if (pageTable==null)
            return;
        for (int i=0; i<pageTable.length; i++)
            UserKernel.addAvailPhysPage(pageTable[i].ppn);
    }    

    /**
     * Initialize the processor's registers in preparation for running the
     * program loaded into this process. Set the PC register to point at the
     * start function, set the stack pointer register to point at the top of
     * the stack, set the A0 and A1 registers to argc and argv, respectively,
     * and initialize all other registers to 0.
     */
    public void initRegisters() {
        Processor processor = Machine.processor();

        // by default, everything's 0
        for (int i=0; i<processor.numUserRegisters; i++)
            processor.writeRegister(i, 0);

        // initialize PC and SP according
        processor.writeRegister(Processor.regPC, initialPC);
        processor.writeRegister(Processor.regSP, initialSP);

        // initialize the first two argument registers to argc and argv
        processor.writeRegister(Processor.regA0, argc);
        processor.writeRegister(Processor.regA1, argv);
    }

    public static final int
    syscallHalt = 0,
    syscallExit = 1,
    syscallExec = 2,
    syscallJoin = 3,
    syscallCreate = 4,
    syscallOpen = 5,
    syscallRead = 6,
    syscallWrite = 7,
    syscallClose = 8,
    syscallUnlink = 9;

    /**
     * Handle a syscall exception. Called by <tt>handleException()</tt>. The
     * <i>syscall</i> argument identifies which syscall the user executed:
     *
     * <table>
     * <tr><td>syscall#</td><td>syscall prototype</td></tr>
     * <tr><td>0</td><td><tt>void halt();</tt></td></tr>
     * <tr><td>1</td><td><tt>void exit(int status);</tt></td></tr>
     * <tr><td>2</td><td><tt>int  exec(char *name, int argc, char **argv);
     * 								</tt></td></tr>
     * <tr><td>3</td><td><tt>int  join(int pid, int *status);</tt></td></tr>
     * <tr><td>4</td><td><tt>int  creat(char *name);</tt></td></tr>
     * <tr><td>5</td><td><tt>int  open(char *name);</tt></td></tr>
     * <tr><td>6</td><td><tt>int  read(int fd, char *buffer, int size);
     *								</tt></td></tr>
     * <tr><td>7</td><td><tt>int  write(int fd, char *buffer, int size);
     *								</tt></td></tr>
     * <tr><td>8</td><td><tt>int  close(int fd);</tt></td></tr>
     * <tr><td>9</td><td><tt>int  unlink(char *name);</tt></td></tr>
     * </table>
     * 
     * @param	syscall	the syscall number.
     * @param	a0	the first syscall argument.
     * @param	a1	the second syscall argument.
     * @param	a2	the third syscall argument.
     * @param	a3	the fourth syscall argument.
     * @return	the value to be returned to the user.
     */
    public int handleSyscall(int syscall, int a0, int a1, int a2, int a3) {
        switch (syscall) {
        case syscallHalt:
            return handleHalt();

            // HANDLE MORE SYSCALL CASES
        case syscallExit:
            return handleExit(a0);
        case syscallCreate:
            return handleCreat(a0);
        case syscallOpen:
            return handleOpen(a0);
        case syscallRead:
            return handleRead(a0, a1, a2);
        case syscallWrite:
            return handleWrite(a0, a1, a2);
        case syscallClose:
            return handleClose(a0);
        case syscallUnlink:
            return handleUnlink(a0);
        case syscallExec:
            return handleExec(a0, a1, a2);
        case syscallJoin:
            int ret = handleJoin(a0, a1);
            return ret;

        default:
            Lib.debug(dbgProcess, "Unknown syscall " + syscall);
        Lib.assertNotReached("Unknown system call!");
        }
        return 0;
    }

    /**
     * Handle the halt() system call. 
     */
    private int handleHalt() {
        if (pid != 0)
            return -1;
        Machine.halt();

        Lib.assertNotReached("Machine.halt() did not halt machine!");
        return 0;
    }

    private int handleCreat(int charPtr) {
        if (!isValidPointer(charPtr))
            return -1;
        String filename = readVirtualMemoryString(charPtr, fileNameMaxSize);
        if (filename == null)
            return -1;

        OpenFile openfile = ThreadedKernel.fileSystem.open(filename, true);
        if (openfile == null)
            return -1;

        int fileD = nextFileDescriptor();
        openFileTable[fileD] = openfile;

        return fileD;
    }

    private int handleOpen(int charPtr) {
        if (!isValidPointer(charPtr))
            return -1;
        String filename = readVirtualMemoryString(charPtr, fileNameMaxSize);
        if (filename == null)
            return -1;

        OpenFile openfile = ThreadedKernel.fileSystem.open(filename, false);
        if (openfile == null)
            return -1;

        int fileD = nextFileDescriptor();
        openFileTable[fileD] = openfile;

        return fileD;
    }

    private int handleRead(int fileD, int bufPtr, int size) {
        
        //TestStubs.debugPrint("HANDLING READING FROM fileD="+fileD);
        
        if (!isValidFileDescriptor(fileD) || fileD==1 || size<0 || !isValidPointer(bufPtr)) {
            //TestStubs.debugPrint("Not a valid fileDescriptor!");
            return -1;
        }

        OpenFile openfile = openFileTable[fileD];
        if (openfile == null) {
            TestStubs.debugPrint("Not an openfile");
            return -1;
        }

        int bytesRead, desiredBytes, totalBytesWritten, bytesWritten;
        bytesRead = totalBytesWritten = bytesWritten = 0;
        int readSize = 0x400;
        byte[] bytes = new byte[readSize];

        while (totalBytesWritten != size) {
            desiredBytes = Math.min(readSize, size-totalBytesWritten);
            bytesRead = openfile.read(bytes, 0, desiredBytes);
            if (bytesRead == -1 || bytesRead == 0) {
                //TestStubs.debugPrint("Did not read the amount of bytes requested...");
                return -1; 
            }

            bytesWritten = writeVirtualMemory(bufPtr+totalBytesWritten, bytes, 0, bytesRead);
            // You reached EOF or empty stream when trying to read from openfile
            if (bytesRead != desiredBytes)
                break;
            // If you get the wrong number of bytes written, return -1
            else if (bytesWritten != bytesRead) {
                TestStubs.debugPrint("Could not write all the bytes to VM!");
                return -1;
            } else
                totalBytesWritten += bytesWritten;
        }
        return totalBytesWritten;
    }

    private int handleWrite(int fileD, int bufPtr, int size) {
        if (!isValidFileDescriptor(fileD) || fileD==0 || size<0 || !isValidPointer(bufPtr))
            return -1;

        
        
        OpenFile openfile = openFileTable[fileD];
        if (openfile == null)
            return -1;

        int bytesRead, totalBytesWritten, bytesWritten;
        bytesRead = totalBytesWritten = bytesWritten = 0;
        int readSize = 0x400;
        byte[] bytes = new byte[readSize];

        while (totalBytesWritten != size) {
            int desiredBytes = Math.min(readSize, size-totalBytesWritten);
            bytesRead = readVirtualMemory(bufPtr+totalBytesWritten, bytes, 0, desiredBytes);
            if (desiredBytes != bytesRead) // Return error in case readVM didn't happen correctly
                return -1;
            
            if (testing && testListener != null && testListener.captureThisfileF(fileD)) {
                testListener.captureWriteStrings(bytes, bytesRead);
                bytesWritten = bytesRead;
            } else {
                bytesWritten = openfile.write(bytes, 0, bytesRead);
            }
            if (bytesWritten != bytesRead)  // Return error if we didn't write in the desired amount
                return -1;
            else
                totalBytesWritten += bytesWritten;
        }
        return size;
    }

    private int handleClose(int fileD) {
        if (!isValidFileDescriptor(fileD))
            return -1;

        OpenFile openfile = openFileTable[fileD];
        if (openfile == null)
            return -1;

        openfile.close();
        openFileTable[fileD] = null;
        return 0;
    }

    private int handleUnlink(int charPtr) {
        if (!isValidPointer(charPtr))
            return -1;
        String filename = readVirtualMemoryString(charPtr, fileNameMaxSize);
        if (filename == null)
            return -1;

        if (!ThreadedKernel.fileSystem.remove(filename))
            return -1;
        else 
            return 0;
    }

    /**
     * Syscall handler for exit(int status);
     */
    private int handleExit(int status) {
        sysCallMultiprocessingLock.acquire();
        terminated = true;
        exitStatus = status; //set exit status code from your argument

        for (UserProcess child : children)
            child.parent = null;
        children.clear();
        
        
        for (int i = 0; i < openFileTable.length; i++)
            handleClose(i); //close all the file descriptors
        unloadSections(); //return memory

        activePIDs--;
        if (activePIDs <= 0 && !testing) {
            Kernel.kernel.terminate();
        }
        coff.close();
        sysCallMultiprocessingLock.release();

        UThread.finish(); //REMOVE FROM RUN QUEUE

        return 0;
    }

    /**
     * Syscall handler for join(int processID, int *status);
     * @return -1 if pid is not child, 1 on successful exit, 0 on unhandled exception
     */
    private int handleJoin(int pid, int stat) {
        UserProcess theChild = null;

        sysCallMultiprocessingLock.acquire();
        for (UserProcess c : children)
            if (c.pid == pid) {
                theChild = c;
                break;
            }
        if (theChild == null) {
            sysCallMultiprocessingLock.release();
            return -1;
        }

        if (theChild.terminated) {
            writeVirtualMemory(stat, Lib.bytesFromInt(theChild.exitStatus), 0, 4);
            sysCallMultiprocessingLock.release();
            return theChild.exitCode;	
        }

        sysCallMultiprocessingLock.release();
        theChild.myThread.join();

        writeVirtualMemory(stat, Lib.bytesFromInt(theChild.exitStatus), 0, 4);
        return theChild.exitCode;
    }

    /**
     * Syscall handler for exec(char *file, int argc, char *argv[]);
     * @return returns child processID
     */
    private int handleExec(int file, int argc, int argv) {

        sysCallMultiprocessingLock.acquire();

        UserProcess newP = newUserProcess();
        String fileS = readVirtualMemoryString(file, fileNameMaxSize);
        if (fileS == null) {
            sysCallMultiprocessingLock.release();
            return -1;
        }
        String argS[] = new String[argc];
        byte[] addr = new byte[4];
        
        for (int i = 0; i < argc; i++) {
            readVirtualMemory(argv+4*i, addr, 0, 4);
            int strAddr = Lib.bytesToInt(addr, 0);
            argS[i] = readVirtualMemoryString(strAddr, fileNameMaxSize);
            if (argS[i] == null) {
                sysCallMultiprocessingLock.release();
                return -1;
            }
        }
        
//        TestStubs.debugPrint("The file name:");
//        TestStubs.debugPrint(fileS);
//        TestStubs.debugPrint("The file arguments:");
//        for (int i=0; i<argS.length; i++) {
//            TestStubs.debugPrint(argS[i]);
//        }

        if (!newP.execute(fileS, argS)) {
            sysCallMultiprocessingLock.release();
            return -1; 				//Is this right?
        }

        newP.parent = this;
        children.add(newP);

        sysCallMultiprocessingLock.release();

        return newP.pid;
    }

    private int nextFileDescriptor() {
        int temp = 2;
        while (openFileTable[temp] != null) {
            temp++;
            if (temp == openFileTable.length)
                increaseOpenFileTable();
        }
        return temp;
    }

    private void increaseOpenFileTable() {
        OpenFile[] temp = new OpenFile[2*openFileTable.length];
        for (int i=0; i<openFileTable.length; i++)
            temp[i] = openFileTable[i];
        openFileTable = temp;
    }

    private boolean isValidFileDescriptor(int fileD) {
        if (fileD < 0 || fileD > openFileTable.length)
            return false;
        return true;
    }

    private boolean isValidPointer(int ptr) {
        return ptr>=0;
    }

    /**
     * Handle a user exception. Called by
     * <tt>UserKernel.exceptionHandler()</tt>. The
     * <i>cause</i> argument identifies which exception occurred; see the
     * <tt>Processor.exceptionZZZ</tt> constants.
     *
     * @param	cause	the user exception that occurred.
     */
    public void handleException(int cause) {
        Processor processor = Machine.processor();

        switch (cause) {
        case Processor.exceptionSyscall:
            int result = handleSyscall(processor.readRegister(Processor.regV0),
                    processor.readRegister(Processor.regA0),
                    processor.readRegister(Processor.regA1),
                    processor.readRegister(Processor.regA2),
                    processor.readRegister(Processor.regA3)
            );
            processor.writeRegister(Processor.regV0, result);
            processor.advancePC();
            break;	

        default:
            int vaddr = processor.readRegister(Processor.regBadVAddr);
            int vpn = Processor.pageFromAddress(vaddr);
            TestStubs.debugPrint("Unexpected exception: " + Processor.exceptionNames[cause] + ", vpn " + vpn);
            Lib.debug(dbgProcess, "Unexpected exception: " + Processor.exceptionNames[cause]);
            TestStubs.enable();
            TestStubs.debugPrint("Unexpected exception: " + Processor.exceptionNames[cause]);
            exitCode = 0;
            handleExit(1);
            Lib.assertNotReached("Unexpected exception");
            break;
        }
    }

    public static void selfTest() {

        
        //syscall_test();
        //join_test();
        //memory_test();
    }

    public static void memory_test() {
        TestStubs.testStartPrint("Entering UserProcess.selfTest (Memory)");
        TestStubs.debugPrint("Number of pages in all of memory: " + Machine.processor().getNumPhysPages());

        UserProcess test1 = UserProcess.newUserProcess();
        TestStubs.debugPrint("VAR, test1: numPages variable before Load is called: " + test1.numPages);
        String[] arg1 = {"0"};
        test1.load("cp.coff", arg1);
        TestStubs.debugPrint("VAR, test1: numPages variable after Load is called: " + test1.numPages);
        test1.loadSections();
        TestStubs.debugPrint("VAR, test1: Checking to see number of PPNs allocated");
        for (int vpn = 0; vpn < test1.numPages; vpn++) 
            TestStubs.debugPrint("--> VPN: " + vpn + ", PPN: " + test1.pageTable[vpn].ppn);
        //reading/writing VM
        byte[] memory = Machine.processor().getMemory();
        int vaddr = 1;
        byte[] data = new byte[pageSize];
        test1.readVirtualMemory(vaddr, data, 0, 1024);
        TestStubs.debugPrint("VAR, test1: ReadingVM: " + " " + data[1021] + " " + data[1022] + " " + data[1023]);
        vaddr = 1024*4-1;
        data = new byte[pageSize];
        data[0] = data[1] = 3;
        int numWritten = test1.writeVirtualMemory(vaddr, data, 0, 2);
        int paddr = Processor.makeAddress(test1.pageTable[4].ppn, 0);
        TestStubs.debugPrint("VAR, test1: Number of bytes written: " + numWritten);
        TestStubs.debugPrint("VAR, test1: WritingVM: " + memory[paddr-1] + " " + memory[paddr] + " " + memory[paddr+1] + " " + memory[paddr + 2]);
        int temp = UserKernel.freeList.size();
        TestStubs.debugPrint("VAR, test1: The list size before return PPN: " + temp);
        test1.unloadSections();
        TestStubs.debugPrint("VAR, test1: Checking to see if all PPN are returned after calling returnPPN: " + (UserKernel.freeList.size() - temp));
        TestStubs.debugPrint("VAR, test1: Verifying the last PPN added into the freeList");
        for (int i = 0; i < test1.numPages; i++) {
            int tempPPN = UserKernel.freeList.get(temp + i);
            TestStubs.debugPrint("--> PPN added at " + (temp + i) + "th position: " + tempPPN);
            UserKernel.freeList.add(temp + i, tempPPN);
        }

        UserProcess test2 = UserProcess.newUserProcess();
        TestStubs.debugPrint("VAR, test2: numPages variable before Load is called: " + test2.numPages);
        String[] arg2 = {"0"};
        test2.load("echo.coff", arg2);
        TestStubs.debugPrint("VAR, test2: numPages variable after Load is called: " + test2.numPages);
        test1.loadSections();
        TestStubs.debugPrint("VAR, test2: Checking to see number of PPNs allocated");
        for (int vpn = 0; vpn < test2.numPages; vpn++) 
            TestStubs.debugPrint("--> VPN: " + vpn + ", PPN: " + test2.pageTable[vpn].ppn);
        temp = UserKernel.freeList.size();
        TestStubs.debugPrint("VAR, test2: The list size before return PPN: " + temp);
        test2.unloadSections();
        TestStubs.debugPrint("VAR, test2: Checking to see if all PPN are returned after calling returnPPN: " + (UserKernel.freeList.size() - temp));
        TestStubs.debugPrint("VAR, test2: Verifying the last PPN added into the freeList");
        for (int i = 0; i < test2.numPages; i++) {
            int tempPPN = UserKernel.freeList.get(temp + i);
            TestStubs.debugPrint("--> PPN added at " + (temp + i) + "th position: " + tempPPN);
            UserKernel.freeList.add(temp + i, tempPPN);
        }

        TestStubs.testDonePrint("Finished UserProcess.selfTest (Memory)");
    }

    public static void syscall_test() {
        TestStubs.testStartPrint("Entering UserProcess.syscall_test()");

        UserProcess test = UserProcess.newUserProcess();
        TestStubs.debugPrint("Testing open/create/close");
        String[] arg1 = {};
        test.execute("syscalls_filesystem.coff", arg1);
        //ThreadedKernel.alarm.waitUntil(1000);
        TestStubs.debugPrint("Called execute(syscalls_filesystem.coff)");
        TestStubs.testDonePrint("Finished UserProcess.syscall_test()");
    }

    public static void join_test() {
        TestStubs.testStartPrint("Entering UserProcess.join_test()");

        UserProcess test = UserProcess.newUserProcess();
        String[] arg1 = {};
        test.execute("jointest.coff", arg1);
        //ThreadedKernel.alarm.waitUntil(1000);
        TestStubs.debugPrint("Called execute(jointest.coff)");
        TestStubs.testDonePrint("Finished UserProcess.jointest()");
    }

    /** The program being run by this process. */
    protected Coff coff;

    private Lock LSLock;

    /** This process's page table. */
    protected TranslationEntry[] pageTable;
    /** The number of contiguous pages occupied by the program. */
    protected int numPages;

    /** The number of pages in the program's stack. */
    protected final int stackPages = 8;

    private int initialPC, initialSP;
    private int argc, argv;

    private static final int pageSize = Processor.pageSize;
    private static final char dbgProcess = 'a';

    // Some other things we need for Task 1.
    OpenFile[] openFileTable;
    static Hashtable<String, Integer> openedFiles = new Hashtable<String, Integer>();
    private static final int fileNameMaxSize = 256;
    private final int maxMemory = Processor.pageSize * Machine.processor().getNumPhysPages();

    /** Contains the next processID to hand out. */
    private static int processIDcounter = 0;
    /** Contains the number of active processes. */
    protected static int activePIDs = 0;
    /** The lock for join, exec and exit */
    private static Lock sysCallMultiprocessingLock = new Lock();

    /** Per process information to maintain process parent-child tree. */
    protected int pid;
    private UserProcess parent;
    private ArrayList<UserProcess> children = new ArrayList<UserProcess>();
    public int exitStatus; //what exit status did you supply?
    public int exitCode = 1; //did you exit with an unhandled exception or not? 1 == normal, 0 == exception
    private boolean terminated = false;
    private UThread myThread = null;
    public static boolean testing = false;
    public TestStubs.CaptureListener testListener = null;
}
