package nachos.vm;

import java.util.*;

import nachos.machine.*;
import nachos.threads.*;
import nachos.userprog.*;

/**
 * A kernel that can support multiple demand-paging user processes.
 */
public class VMKernel extends UserKernel {
    /**
     * Allocate a new VM kernel.
     */
    public VMKernel() {
    }

    /**
     * Initialize this kernel.
     */
    public void initialize(String[] args) {
	super.initialize(args);
	MemoryMap.getMemoryMap();
        Swap.getSwap();
    }

    /**
     * Start running user programs.
     */
    public void run() {
        super.run();
    }

    /**
     * Terminate this kernel. Never returns.
     */
    public void terminate() {
        Swap.getSwap().close();
        super.terminate();
    }

    // dummy variables to make javac smarter
    private static VMProcess dummy1 = null;
    private static final char dbgVM = 'v';

    /**
     * 
     * TEST CODE FOLLOWS
     * 
     */

    /**
     * Test this kernel.
     */ 
    public void selfTest() {
        
        
        
        /**
         * TESTING CONFIGURATION RIGHT HERE:
         */
        int total = 1;                                 //How many tests should we run
        int start = 0;                                  //The first test to run
        //TestStubs.storePrints();                      //store prints until the end
        TestStubs.displayDebugWhileTesting(false);       //shows all the debug prints (with context switches) if set to true
        
        
        //MemoryMap.selfTest();
        //Swap.selfTest();
        
        UserProcess.testing = true;
        TestStubs.debugPrint("SELF TESTING VMKERNEL!");
        
        int passed = 0;
        
        for (int i = 0; i < total; i++) {
            if (runTest(i+start))
                passed += 1;
        }
        
        if (passed != total) {
            TestStubs.debugPrint("SOME TESTS FAILED! " + passed + "/" + total + " passed.");
        } else {
            TestStubs.debugPrint("ALL TESTS PASSED! " + passed + "/" + total + " passed.");
        }
        
        UserProcess.testing = false;
        

        
        //System.out.println(TestStubs.getStoredPrints());
        //TestStubs.clearAndDontStorePrints();
        
        if (passed != total) {
            System.out.flush();
        ///    kernel.terminate();
        }
        
        TestStubs.disable();
    
    }
    
    /**
     * Tests all of em!
     * @return
     */
    public boolean runTest(int which) {
        
        TestStubs.testStartPrint("Running Test " + which);
        
        boolean test = false;
        
        switch(which) {
        case 0:
            test = test0();
            break;
        case 1:
            test = test1();
            break;
        case 2:
            test = test2();
            break;
        case 3:
            test = test3();
            break;
        case 4:
            test = test4();
            break;
        case 5:
            test = test5();
            break;
        case 6:
            test = test6();
            break;
        case 7:
            test = test7();
            break;
        case 8:
            test = test8();
            break;
        case 9:
            test = test9();
            break;
        case 10:
            test = test10();
            break;
        case 11:
            test = test11();
            break;
        case 12:
            test = test12();
            break;
        case 13:
            test = test13();
            break;
        case 14:
            test = test14();
            break;
        case 15:
            test = test15();
            break;
        case 16:
            test = test16();
            break;       
        case 17:
            test = test17();
            break;   
        case 18:
            test = test18();
            break; 
        case 19:
            test = testLOTS();
            break;
        default:
            TestStubs.debugPrint("NO SUCH TEST - " + which);
        }
        
        printSettings();
        
        
 
        if (test)
            TestStubs.testDonePrint("Test " + which + " SUCCESS\n\n");
        else
            TestStubs.testDonePrint("Test " + which + " FAIL\n\n");
        return test;
    }
    
    private boolean runTest(String shellProgram, String[] arguments, String[] outputFragments, int wantedExitCode) {
        
        TestStubs.debugPrint("Running " + shellProgram);
        if (!TestStubs.displayDebugWhileTesting())
            TestStubs.disable();
        else
            TestStubs.enable();
        
        UserProcess process = UserProcess.newUserProcess();
        TestStubs.CaptureListener listener = new TestStubs.CaptureListener(); 
        process.testListener = listener; //Registering event listener
        
        Lib.assertTrue(VMProcess.getActivePIDs() == 0);
        boolean test = process.execute(shellProgram, arguments);
        while (VMProcess.getActivePIDs() != 0) {
            KThread.yield();
        }
        
        if (!TestStubs.displayDebugWhileTesting())
            TestStubs.enable();
        
        TestStubs.debugPrint("Exit code: " + process.exitCode + " (normal == 1, exception == 0) [wanted: "+wantedExitCode+"]");
        TestStubs.debugPrint("Exit status: " + process.exitStatus + " (as returned from int main)");
        
        if (outputFragments != null)
            test = listener.checkWrittenStrings(outputFragments) && test;
        else
            TestStubs.debugPrint("This was written to STDOUT: \n" + listener.getWrittenStrings());
        
        return (process.exitCode == wantedExitCode) && test;
    }
    
    private boolean runTest2(String shellProgram1, String[] arguments1, String[] outputFragments1, int wantedExitCode1, 
            String shellProgram2, String[] arguments2, String[] outputFragments2, int wantedExitCode2) {
        
        TestStubs.debugPrint("Running " + shellProgram1 + " and " + shellProgram2);
        if (!TestStubs.displayDebugWhileTesting())
            TestStubs.disable();
        else
            TestStubs.enable();
        
        
        UserProcess process1 = UserProcess.newUserProcess();
        UserProcess process2 = UserProcess.newUserProcess();
        TestStubs.CaptureListener listener1 = new TestStubs.CaptureListener();
        TestStubs.CaptureListener listener2 = new TestStubs.CaptureListener(); 
        process1.testListener = listener1; //Registering event listener
        process2.testListener = listener2; //Registering event listener
        
        Lib.assertTrue(VMProcess.getActivePIDs() == 0);
        boolean test1 = process1.execute(shellProgram1, arguments1);
        boolean test2 = process2.execute(shellProgram2, arguments2);
        while (VMProcess.getActivePIDs() != 0) {
            KThread.yield();
        }
        
        if (!TestStubs.displayDebugWhileTesting())
            TestStubs.enable();
        
        TestStubs.debugPrint(shellProgram1 + " Exit code: " + process1.exitCode + " (normal == 1, exception == 0) [wanted: "+wantedExitCode1+"]");
        TestStubs.debugPrint(shellProgram1 + " Exit status: " + process1.exitStatus + " (as returned from int main)");
        TestStubs.debugPrint(shellProgram2 + " Exit code: " + process2.exitCode + " (normal == 1, exception == 0) [wanted: "+wantedExitCode2+"]");
        TestStubs.debugPrint(shellProgram2 + " Exit status: " + process2.exitStatus + " (as returned from int main)");
        
        if (outputFragments1 != null)
            test1 = listener1.checkWrittenStrings(outputFragments1) && test1;
        else
            TestStubs.debugPrint(shellProgram1 + " This was written to STDOUT: \n" + listener1.getWrittenStrings());
        
        if (outputFragments2 != null)
            test2 = listener2.checkWrittenStrings(outputFragments2) && test2;
        else
            TestStubs.debugPrint(shellProgram2 + " This was written to STDOUT: \n" + listener2.getWrittenStrings());
        
        return (process1.exitCode == wantedExitCode1) && test1 && 
                    (process2.exitCode == wantedExitCode2) && test2;
    }
    
    private boolean test0() {
        
        TestStubs.debugPrint("Testing echo of arguments page");
        String shellProgram = "echo.coff";
        String[] arguments = new String[] {"woo", "woooow" };
        String[] outputFragments = {"2 arguments\n" +
        		"arg 0: woo\n" +
        		"arg 1: woooow\n" +
        		""};
        return runTest(shellProgram, arguments, outputFragments, 1);
        
    }
    
    private boolean test1() {
        TestStubs.debugPrint("Testing reading from file into 10 char buffer.");
        String shellProgram = "read1.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"Starting read1 test\n" + 
                        "src is NOT suppose to be -1\n" +
                        "src is: 2\n" +
                        ""};
        return runTest(shellProgram, arguments, outputFragments, 1);
    }
    
    
    private boolean test2() {
        TestStubs.debugPrint("Testing reading TWICE from file into 10 char buffer.");
        String shellProgram = "read2.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"Starting read2 test, does 2 reads\n" + 
                        "src is NOT suppose to be -1\n" +
                        "src is: 2\n" +
                        "read(src, buf, 9) returns 9\n" +
                        "buf after first read: test file\n" +
                        "second read(src, buf, 9) returns 9\n" +
                        "buf after second read:  123 end\n\n"};
        return runTest(shellProgram, arguments, outputFragments, 1);
    }
    
    private boolean test3() {
        TestStubs.debugPrint("Testing reading TWICE, writing ONCE, 10 char buffer.");
        String shellProgram = "read2write1.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"Starting read2 write1 test, does 2 reads and 1 write\n" +
        		"src is NOT suppose to be -1\n" +
        		"src is: 2\n" +
        		"read(src, buf, 9) returns 9\n" +
        		"buf after first read: test file\n" +
        		"second read(src, buf, 9) returns 9\n" +
        		"buf after second read:  123 end\n" +
        		"\n" +
        		"created test2.txt\n" +
        		"creat is suppose to >= 0\n" +
        		"creat returns 3\n" +
        		"writing to test2.txt\n" +
        		"write returned 10\n" +
        		"write finished\n" +
        		"test2.txt should contain:  123 end\n" +
        		"\n" +
        		"opening test2.txt\n" +
        		"read(fd, buf, 9) returns  123 end\n\n"};
        return runTest(shellProgram, arguments, outputFragments, 1);
    }
    private boolean test4() {
        TestStubs.debugPrint("Scanning through file. THIS TEST DESIGNED FOR 2 PAGES!");
        String shellProgram = "scanRead.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"Starting scanRead test, scans twice\n" +
        		"Number of physical pages should be 2\n" +
        		"open is suppose to >= 0\n" +
        		"open returns 2\n" +
        		"buf 0: This is a \n" +
        		"buf 1: test. Repe\n" +
        		"buf 2: at 1234 Re\n" +
        		"buf 3: peat 1234 \n" +
        		"buf 4: Repeat 123\n" +
        		"buf 5: 4 Repeat 1\n"};
        return runTest(shellProgram, arguments, outputFragments, 1);
    }
    private boolean test5() {
        TestStubs.debugPrint("Stress test - does 5 reads/writes 15 times. DESIGNED FOR 4 PAGES!");
        String shellProgram = "stressTest.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"Starting single stress test, does 5 R/W for 15 iterations\n" +
        		"Num physical pages is suppose to be 4\n" +
        		"Iteration 0, there should be 15 iterations\n" +
        		"Iteration 1, there should be 15 iterations\n" +
        		"Iteration 2, there should be 15 iterations\n" +
        		"Iteration 3, there should be 15 iterations\n" +
        		"Iteration 4, there should be 15 iterations\n" +
        		"Iteration 5, there should be 15 iterations\n" +
        		"Iteration 6, there should be 15 iterations\n" +
        		"Iteration 7, there should be 15 iterations\n" +
        		"Iteration 8, there should be 15 iterations\n" +
        		"Iteration 9, there should be 15 iterations\n" +
        		"Iteration 10, there should be 15 iterations\n" +
        		"Iteration 11, there should be 15 iterations\n" +
        		"Iteration 12, there should be 15 iterations\n" +
        		"Iteration 13, there should be 15 iterations\n" +
        		"Iteration 14, there should be 15 iterations\n" +
        		"finished stress test\n"};
        return runTest(shellProgram, arguments, outputFragments, 1);
    }
    private boolean test6() {
        TestStubs.debugPrint("Writes to an illegal/bad virtual address");
        String shellProgram = "VM_badaddr.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"dest is NOT suppose to be -1\n" +
        		"dest is: 2\n" +
        		"write(dst, (void*) 0x7FFE1234, 0x7FFFFFFF) is suppose to be less than or equal to 0\n" +
        		"write(dst, (void*) 0x7FFE1234, 0x7FFFFFFF) is: -1\n"};
        return runTest(shellProgram, arguments, outputFragments, 1);
    }
    private boolean test7() {
        TestStubs.debugPrint("Writes to a readOnly page - this should fail our process.");
        String shellProgram = "VM_write_readonly.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"src is NOT suppose to be -1\n" +
        		"src is: 2\n" +
        		"read(src, (void*) 0x00000000, 4) is suppose to be <= 0\n" +
        		"read(src, (void*) 0x00000000, 4) is: -1\n"};
        return runTest(shellProgram, arguments, outputFragments, 1);
    }
    private boolean test8() {
        TestStubs.debugPrint("BIG write test - writes over multiple pages with 2000 byte buffer.");
        String shellProgram = "writeBigSize.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"Starting writeBigSize test, writes more than max page size\n" +
        		"creat is suppose to >= 0\n" +
        		"creat returns 2\n" +
        		"writing to test10.txt\n" +
        		"write wrote 2000\n" +
        		"write finished\n" +
        		"read(fd, buf, 2000) returns 2000\n" +
        		"\n" +
        		"Assertion failed: line 151 file printf.c\n"};
        return runTest(shellProgram, arguments, outputFragments, 1);
    }
    private boolean test9() {
        TestStubs.debugPrint("Tests writing to virtual memory fuction (Not sure about this test... check .c code!)");
        String shellProgram = "writeVMTest.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"Starting read2 test, does 2 reads\n" +
        		"should be using 1 physical page\n" +
        		"src is NOT suppose to be -1\n" +
        		"src is: 2\n" +
        		"reading 10 times\n" +
        		"buf after 1th: Repeat 123\n" +
        		"buf after 2th: Repeat 123\n" +
        		"buf after 3th: Repeat 123\n" +
        		"buf after 4th: Repeat 123\n" +
        		"buf after 5th: Repeat 123\n" +
        		"buf after 6th: Repeat 123\n" +
        		"buf after 7th: Repeat 123\n" +
        		"buf after 8th: Repeat 123\n" +
        		"buf after 9th: Repeat 123\n" +
        		"buf after 10th: Repeat 123\n"};
        return runTest(shellProgram, arguments, outputFragments, 1);
    }
    private boolean test10() {
        TestStubs.debugPrint("Forces swapping to happen by rereading pages. THIS TEST DESIGNED FOR 2 PAGES!");
        String shellProgram = "forceSwap.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"Starting forceswap test, does re-read on swapped file\n" +
        		"Num physical pages is suppose to be 2\n" +
        		"src is NOT suppose to be -1\n" +
        		"src is: 2\n" +
        		"write(src, \"testing12\", 9) returns 9\n" +
        		"src2 is NOT suppose to be -1\n" +
        		"src2 is: 2\n" +
        		"write(src2, \"blahblah\", 8) returns 8\n" +
        		"closing testTmp.txt, this should return 0\n" +
        		"closing returns 0\n" +
        		"unlink should return 0\n" +
        		"unlink returns 0\n" +
        		"src2 is NOT suppose to be -1\n" +
        		"src2 is: 2\n" +
        		"write(src2, \"blahblah\", 8) returns 8\n" +
        		"closing testTmp.txt, this should return 0\n" +
        		"closing returns 0\n" +
        		"unlink should return 0\n" +
        		"unlink returns 0\n" +
        		"wrote 2 files and deleted them\n" +
        		"src should be swapped out, now reading it\n" +
        		"read should be > 0\n" +
        		"read(src, buf, 9) returns 9\n" +
        		"buf after read: testing12\n"};
        return runTest(shellProgram, arguments, outputFragments, 1);
    }
    private boolean test11() {
        TestStubs.debugPrint("Runs MatMult");
        String shellProgram = "matmult.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"C[19][19] = 7220\n"};
                
        return runTest(shellProgram, arguments, outputFragments, 1);
    }
    private boolean test12() {
        TestStubs.debugPrint("Runs Longarray");
        String shellProgram = "longarray.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"first element in array (expect b): b\n" +
                        "last element in array (expect b): b\n"};
                
        return runTest(shellProgram, arguments, outputFragments, 1);
    }

    private boolean test13() {
        TestStubs.debugPrint("Runs Longarray AND MatMult");
        String shellProgram1 = "longarray.coff";
        String[] arguments1 = new String[] { };
        String outputFragments1[] = {"first element in array (expect b): b\n" +
                        "last element in array (expect b): b\n"};
        
        String shellProgram2 = "matmult.coff";
        String[] arguments2 = new String[] { };
        String outputFragments2[] = {"C[19][19] = 7220\n"};        
        return runTest2(shellProgram1, arguments1, outputFragments1, 1,
                shellProgram2, arguments2, outputFragments2, 1);
    }
    private boolean test14() {
        TestStubs.debugPrint("join_abnormal.c - execs join.coff, joins on it.");
        String shellProgram = "join_abnormal.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"retVal: -1\n" + 
                        "retVal2: -1\n" + 
                        ""};
        return runTest(shellProgram, arguments, outputFragments, 1);
    }
    private boolean test15() {
        TestStubs.debugPrint("Joining on pid of -1.");
        String shellProgram = "join_abnormal2.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"joining with exec returning -1\n" + 
                            "retVal2: -1\n"};
        return runTest(shellProgram, arguments, outputFragments, 1);
    }
    private boolean test16() {
        TestStubs.debugPrint("join_test2: Executing and Joining on non-existant .coff file.");
        String shellProgram = "join_test2.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"retVal: -1\n" +
                            "retVal2: -1\n"};
        return runTest(shellProgram, arguments, outputFragments, 1);
    }
    private boolean test17() {
        TestStubs.debugPrint("Executing previous test (join_test2) and joining on the PID.");
        String shellProgram = "join_test3.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"" +
        		"retVal: ", 
        		"retVal2: 1\n" +
        		"status1: 888"};
		
        return runTest(shellProgram, arguments, outputFragments, 1);
    }
    private boolean test18() {
        TestStubs.debugPrint("Runs Joining on MatMult...");
        String shellProgram = "join_test4.coff";
        String[] arguments = new String[] { };
        String[] outputFragments = {"retVal: ", 
                "retVal2 (should be 1): 1\n" +
                "status return value (should be 7220): 7220\n"};
                
        return runTest(shellProgram, arguments, outputFragments, 1);
    }
    private boolean testLOTS() {
        TestStubs.debugPrint("Runs Longarray AND MatMult");
        
        String shellProgram1 = "join_test3.coff";
        String[] arguments1 = new String[] { };
        String[] outputFragments1 = {"" +
                "retVal: ", 
                "retVal2: 1\n" +
                "status1: 888"};
        
        String shellProgram2 = "echo.coff";
        String[] arguments2 = new String[] {"Niels", "was", "here" };
        String[] outputFragments2 = {
                        "3 arguments", 
                        "arg 0: Niels",
                        "arg 1: was",
                        "arg 2: here"};
        
        return runTest2(shellProgram1, arguments1, outputFragments1, 1,
                shellProgram2, arguments2, outputFragments2, 1);
    }

    public void printSettings() {
        
        TestStubs.testStartPrint("Current VM State:");
            TestStubs.debugPrint("Number of Pages: " + Machine.processor().getNumPhysPages());
            
       TestStubs.testDonePrint("");
    }

}
