package nachos.threads;

import java.util.PriorityQueue;
import nachos.machine.*;
import nachos.threads.TestStubs.MustReachAssertion;

/**
 * Uses the hardware timer to provide preemption, and to allow threads to sleep
 * until a certain time.
 */
public class Alarm {
    
    // This is the queue that waiting threads reside on
    private PriorityQueue<AlarmWaiter> waitUntilQueue = new PriorityQueue<AlarmWaiter>();
    
    /**
     * Allocate a new Alarm. Set the machine's timer interrupt handler to this
     * alarm's callback.
     *
     * <p><b>Note</b>: Nachos will not function correctly with more than one
     * alarm.
     */
    public Alarm() {
	Machine.timer().setInterruptHandler(new Runnable() {
	    public void run() { timerInterrupt(); }
	});
    }

    /**
     * The timer interrupt handler. This is called by the machine's timer
     * periodically (approximately every 500 clock ticks). Causes the current
     * thread to yield, forcing a context switch if there is another thread
     * that should be run.
     */
    public void timerInterrupt() {
        long currentTime = Machine.timer().getTime();
        AlarmWaiter nextWaiter = waitUntilQueue.peek();
        
        while ((nextWaiter != null) && (nextWaiter.getWakeTime() <= currentTime)) {
            nextWaiter.getKThread().ready();
            waitUntilQueue.remove();
            nextWaiter = waitUntilQueue.peek();
        }
        
	KThread.yield();
    }

    /**
     * Put the current thread to sleep for at least <i>x</i> ticks,
     * waking it up in the timer interrupt handler. The thread must be
     * woken up (placed in the scheduler ready set) during the first timer
     * interrupt where
     *
     * <p><blockquote>
     * (current time) >= (WaitUntil called time)+(x)
     * </blockquote>
     *
     * @param	x	the minimum number of clock ticks to wait.
     *
     * @see	nachos.machine.Timer#getTime()
     */
    public void waitUntil(long x) {
        boolean intStatus = Machine.interrupt().disable();
        
        long wakeTime = Machine.timer().getTime() + x;
        AlarmWaiter wrapper = new AlarmWaiter(KThread.currentThread(), wakeTime);
	waitUntilQueue.add(wrapper);
        KThread.sleep();
        Machine.interrupt().restore(intStatus);
    }

    /**
     * The Alarm class makes an AlarmWaiter for each thread
     * that wants to wait on the Alarm.
     */
    public class AlarmWaiter implements Comparable<AlarmWaiter> {

        // This is a reference to the KThread that wants to wait
        private KThread myThread;
        // This is the time at which myThread wants to wake up
        private long wakeTime;
        
        /**
         * Construct an AlarmWaiter object
         * 
         * @param thread The thread that wants to wake up
         * @param time The time that the thread wants to wake up
         */
        public AlarmWaiter(KThread thread, long time) {
            myThread = thread;
            wakeTime = time;
        }
        
        public int compareTo(AlarmWaiter waiter) {
            long waiterTime = waiter.getWakeTime();
            if (this.wakeTime > waiterTime)
                return 1;
            else if (this.wakeTime == waiterTime)
                return 0;
            else
                return -1;
        }
        
        /**
         * @return the time that the KThead should wake up at
         */
        public long getWakeTime() {
            return wakeTime;
        }
        
        /**
         * @return the KThread that chose to wait 
         */
        public KThread getKThread() {
            return myThread;
        }
        
    }
    
    /**
     * Tests the Communicator class.
     * Please see the Design Doc for Project 1
     */
    public static void selfTest() {
        TestStubs.testStartPrint("Entering Alarm.selfTest");
        ran = TestStubs.getAssertReachedStart("Alarm Test 2a");
        ran2 = TestStubs.getAssertReachedStart("Alarm Test 2b");
        assertt1 = TestStubs.getAssertReachedStart("Alarm Test 3 t1");
        assertt2 = TestStubs.getAssertReachedStart("Alarm Test 3 t2");
        assertt3 = TestStubs.getAssertReachedStart("Alarm Test 3 t3");
        test1();
        test2();
        test3();
        TestStubs.testDonePrint("Finished Alarm.selfTest");
    }
    
    /**
     * Tests a simple case where we sleep for 1500 ticks.
     * Do we sleep for 1500 ticks or more?
     */
    private static void test1() {
        TestStubs.testStartPrint("Running Alarm test1");
        MustReachAssertion test1ran = TestStubs.getAssertReachedStart("Alarm Test 1");
        long starttime = Machine.timer().getTime();
        long sleeptime = 1500;
        ThreadedKernel.alarm.waitUntil(sleeptime);
        long wakeTime = Machine.timer().getTime();
        Lib.assertTrue(starttime + sleeptime < wakeTime);
        test1ran.reached();   
        TestStubs.testDonePrint("Finished Alarm test1");
    }
    
    static MustReachAssertion ran;
    static MustReachAssertion ran2;
    
    /**
     * Tests the case where two threads both sleep for 1500 ticks.
     * Do both sleep for 1500 or more ticks?
     */
    private static void test2() {
        TestStubs.testStartPrint("Running Alarm test2");
        KThread t1 = new KThread(new Runnable() {
            public void run() {
                long starttime = Machine.timer().getTime();
                long sleeptime = 1500;
                
                ThreadedKernel.alarm.waitUntil(sleeptime);
                long wakeTime = Machine.timer().getTime();
                Lib.assertTrue(starttime + sleeptime < wakeTime);
                ran2.reached();
            }
        });
        t1.fork();
        long starttime = Machine.timer().getTime();
        long sleeptime = 1500;
        ThreadedKernel.alarm.waitUntil(sleeptime);
        long wakeTime = Machine.timer().getTime();
        Lib.assertTrue(starttime + sleeptime < wakeTime);
        ran.reached();
        while (!ran2.hasReached())
            KThread.yield();
        TestStubs.testDonePrint("Finished Alarm test2");
    }
    
    static MustReachAssertion assertt1;
    static MustReachAssertion assertt2;
    static MustReachAssertion assertt3;

    /**
     * Tests the case where 3 independent threads sleep for 1500, 300 and 4500 ticks.
     * Does each sleep for the allocated amount of time?
     * Does the waitUntilQueue contain the right amount of threads 
     * at each point during the simulation?
     * Does the right thread get popped off when woken up?
     */
    private static void test3() {
        TestStubs.testStartPrint("Running Alarm test3");
        long starttime = Machine.timer().getTime();
        
        KThread th1 = new KThread(new Runnable() {
            public void run() {
                long starttime1 = Machine.timer().getTime();
                long sleeptime = 1500;
                ThreadedKernel.alarm.waitUntil(sleeptime);
                long wakeTime = Machine.timer().getTime();
                Lib.assertTrue(starttime1 + sleeptime < wakeTime); //check sleeping duration
                assertt1.reached();
            }
        });
        KThread th2 = new KThread(new Runnable() {
            public void run() {
                long starttime2 = Machine.timer().getTime();
                long sleeptime = 3000;
                ThreadedKernel.alarm.waitUntil(sleeptime);
                long wakeTime = Machine.timer().getTime();
                Lib.assertTrue(starttime2 + sleeptime < wakeTime); //check sleeping duration
                assertt2.reached();
            }
        });
        KThread th3 = new KThread(new Runnable() {
            public void run() {
                long starttime3 = Machine.timer().getTime();
                long sleeptime = 4500;
                ThreadedKernel.alarm.waitUntil(sleeptime);
                long wakeTime = Machine.timer().getTime();
                Lib.assertTrue(starttime3 + sleeptime < wakeTime); //check sleeping duration
                assertt3.reached();
            }
        });
        th1.fork();
        th2.fork();
        th3.fork();
        
        //check that all the threads have been run
        while(!assertt1.hasReached() || !assertt2.hasReached() || !assertt3.hasReached()) {
            //Check the correct ordering and size of the qait queue:
            if (ThreadedKernel.alarm.waitUntilQueue.peek() == null) {
                //empty block here
            } else if (ThreadedKernel.alarm.waitUntilQueue.peek().getKThread() == th1) {
                Lib.assertTrue(ThreadedKernel.alarm.waitUntilQueue.size() == 3);
            } else if (ThreadedKernel.alarm.waitUntilQueue.peek().getKThread() == th2) {
                Lib.assertTrue(ThreadedKernel.alarm.waitUntilQueue.size() == 2);
            } else if (ThreadedKernel.alarm.waitUntilQueue.peek().getKThread() == th3) {
                Lib.assertTrue(ThreadedKernel.alarm.waitUntilQueue.size() == 1);
            }
            
            if (starttime + 4500 + 1000 < Machine.timer().getTime()) {
                Lib.assertNotReached(); //Checks that they all get woken up reasonably soon.
            }
            
            KThread.yield();
        }
        TestStubs.testDonePrint("Finished Alarm test3");
        
    }
    
}
