package nachos.threads;

import java.awt.BorderLayout;
import java.awt.Font;
import java.util.ArrayList;

import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextPane;

import nachos.machine.Lib;
import nachos.machine.Machine;
import nachos.machine.TranslationEntry;

public class TestStubs {
    public static final char dbgTest = '1';
    private static int dbgPrintLevels = 0;
    private static boolean enabled = false;
    private static boolean storePrints = false;
    private static StringBuilder savedPrints = new StringBuilder();
    private static boolean displayWhileTesting = true;
    
    public static void storePrints() {
        storePrints = true;
    }
    public static void clearAndDontStorePrints() {
        storePrints = false;
        savedPrints = new StringBuilder();
    }
    public static String getStoredPrints() {
        return savedPrints.toString();
    }
    
    public static boolean disable() {
        boolean currVal = enabled;
        enabled = false;
        return currVal;
    }
    public static boolean enable() {
        boolean currVal = enabled;
        enabled = true;
        return currVal;
    }
    public static void restore(boolean stat) {
        enabled = stat;
    }
    public static boolean displayDebugWhileTesting() {
        return displayWhileTesting;
    }
    public static void displayDebugWhileTesting(boolean shouldWe) {
        displayWhileTesting = shouldWe;
    }
    
    public static void testStartPrint(String message) {
        if (!enabled)
            return;
        printHelper(message, 1, false);
        dbgPrintLevels++;
    }
    
    public static void testDonePrint(String message) {
        if (!enabled)
            return;
        if (dbgPrintLevels > 0)
            dbgPrintLevels--;
        else
            dbgPrintLevels = 0;
        if (dbgPrintLevels == 0)
            printHelper(message, 2, true);
        else
            printHelper(message, 2, false);
    }
    
    public static void debugPrint(String message) {
        if (!enabled)
            return;
        printHelper(message, 0, false);
    }
    
    public static void printTLBContents() {
        if (!enabled)
            return;
        testStartPrint("printing TLB:");
        for (int t = 0; t < Machine.processor().getTLBSize(); t++) {
            TranslationEntry tw = Machine.processor().readTLBEntry(t);
            debugPrint("TLB["+t+"]: ("+tw.vpn+"->"+tw.ppn+") = (dirty="+tw.dirty+", used="+tw.used+", valid="+tw.valid+", readOnly="+tw.readOnly+")");
        }
        testDonePrint("");
        
    }
    
    private static void printHelper(String message, int arrow, boolean newLine) {
        StringBuilder bldr = new StringBuilder();
        
        if (dbgPrintLevels != 0) {
            for (int i = 0; i < dbgPrintLevels; i++)
                bldr.append("   ");
        }
        if (arrow == 1)
            bldr.append("-->>");
        else if (arrow == 2)
            bldr.append("<<--");
        else
            bldr.append("----");
        bldr.append(" ");
        bldr.append(message);
        if (newLine)
            bldr.append("\n");
        
        
        if (storePrints) {
            savedPrints.append(bldr.append("\n"));
        } else if (DebugGUI.useGUI) {
            bldr.append("\n");
            myGui.addText(bldr.toString());
        } else {
            Lib.debug(TestStubs.dbgTest, bldr.toString());
        }
    }
    
    /**
     * This is a class that represents an assertion
     * that needs to be reached for the program to run correctly.
     * Acquire one though Lib.getAssertReachedStart() and set one
     * as finished by using the reached() method.
     * @author Niels Joubert
     *
     */
    public static class MustReachAssertion {
        private boolean hasReached = false;
        private String name;
        public MustReachAssertion(String name) {
            this.name = name;
        }
        public void reached() {
            hasReached = true;
        }
        public boolean hasReached() {
            return hasReached;
        }
        public String getTitle() { return name; }
        
    }
    
    /**
     * Returns the list of assertions for places that must have been
     * reached. This is used in ThreadedKernel's selftest method as the
     * actuator to ensure that all pieces of code has been ran as
     * requested.
     * @return ArrayList of assertions
     */
    public static ArrayList<MustReachAssertion> getAssertReachedList() {
        return mustBeTrue;
    }
    /**
     * Creates an assertion for a piece of code that needs to be reached, 
     * initially set to unreached and added to the list of all such assertions.
     * Use this BEFORE the critical piece of code, and use the reached() method
     * on the returned object to indicate when the critical section has been ran.
     * @return
     */
    public static MustReachAssertion getAssertReachedStart(String title) {
        MustReachAssertion e = new MustReachAssertion(title);
        mustBeTrue.add(e);
        return e;
    }
    /**
     * Asserts that all known MustReachAssertions has been reached.
     * Used as the last statement in ThreadedKernel's selfTest() method
     */
    public static void assertAllHasBeenReached() {
        for (MustReachAssertion mustreach : getAssertReachedList()) {
            Lib.assertTrue(mustreach.hasReached, mustreach.getTitle());
        	
        	//if (!mustreach.hasReached())
            //    throw new AssertionFailureError(mustreach.getTitle());
        }
    }
    
    public static void assertTrue(boolean status) {
        if (enabled)
            Lib.assertTrue(status);
    }
    
    public static class DebugGUI {
        public static final boolean useGUI = false;
        
        JFrame debugframe;
        JFrame tables;
        
        JTextArea area;
        JTextArea tlbA;
        JTextArea IPTA;
        JTextArea CMA;
        public DebugGUI() {
            if (useGUI)
                createGUI();
        }
        
        private void createGUI() {
            debugframe = new JFrame("DEBUG GUI!");
            
            area = new JTextArea();
            //area.setFont(new Font("Verdana", 1, 14));
            area.setFont(new Font("Courier New", 0, 14));
            JScrollPane scrollPane = new JScrollPane(area);
            debugframe.add(scrollPane);

            debugframe.setSize(1200, 800);
            debugframe.setVisible(true);
        
            /*
            tlbA = new JTextArea();
            IPTA = new JTextArea();
            CMA = new JTextArea();
            
            tables = new JFrame("VM Tables!");
            //tables.setLayout(new BoxLayout(tables, BoxLayout.Y_AXIS));
            tables.getContentPane().add(new JScrollPane(tlbA));
            tables.getContentPane().add(new JScrollPane(IPTA));
            tables.getContentPane().add(new JScrollPane(CMA));
            tables.setSize(300, 800);
            tables.setVisible(true);
            */
        }
        
        public void addText(String text) {
            area.append(text);
        }
        
    }
    
    /**
     * An event listener that captures exceptions in a Process.
     * This is designed to trap writes in UserProcess for testing purposes.
     * @author Niels Joubert
     *
     */
    public static class CaptureListener {
        
        public CaptureListener() {
            written = new StringBuilder();
        }
        
        public void captureWriteStrings(byte[] bytes, int bytesRead) {
            if (bytes[bytesRead-1] == 0)
                bytesRead--;
            written.append(new String(bytes, 0, bytesRead));
        }
        
        public String getWrittenStrings() {
            return written.toString();
        }
        
        public boolean checkWrittenStrings(String[] fragments) {
            boolean good = true;
            int startOffset = 0;
            for (int i = 0; i < fragments.length; i++) {
                int iOf = getWrittenStrings().indexOf(fragments[i], startOffset);
                if (iOf == -1) {
                    TestStubs.debugPrint("Expected output MUST CONTAIN:\n=============================\n"+fragments[i]);
                    TestStubs.debugPrint("Actual output:\n==============================\n"+getWrittenStrings());
                } else {
                    startOffset += ((iOf + fragments[i].length()) - 1);
                }
                good = good && (iOf > -1);
            }
            return good;
        }
        
        private StringBuilder written;

        public boolean captureThisfileF(int fileD) {
            
            return (fileD == 1);
            
        }
        
    }
    
    
    public static DebugGUI myGui;
    
   private static ArrayList<MustReachAssertion> mustBeTrue = new ArrayList<MustReachAssertion>();
   
}
