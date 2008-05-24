package nachos.threads;

import nachos.machine.*;
import nachos.threads.TestStubs.MustReachAssertion;

/**
 * An implementation of condition variables that disables interrupt()s for
 * synchronization.
 *
 * <p>
 * You must implement this.
 *
 * @see	nachos.threads.Condition
 */
public class Condition2 {
    /**
     * Allocate a new condition variable.
     *
     * @param	conditionLock	the lock associated with this condition
     *				variable. The current thread must hold this
     *				lock whenever it uses <tt>sleep()</tt>,
     *				<tt>wake()</tt>, or <tt>wakeAll()</tt>.
     */
    public Condition2(Lock conditionLock) {
        this.conditionLock = conditionLock;
    }

    /**
     * Atomically release the associated lock and go to sleep on this condition
     * variable until another thread wakes it using <tt>wake()</tt>. The
     * current thread must hold the associated lock. The thread will
     * automatically reacquire the lock before <tt>sleep()</tt> returns.
     */
    public void sleep() {
        Lib.assertTrue(conditionLock.isHeldByCurrentThread());
        boolean intStatus = Machine.interrupt().disable();
        waitQueue.waitForAccess(KThread.currentThread());
        conditionLock.release();
        KThread.sleep();       
        Machine.interrupt().restore(intStatus);
        conditionLock.acquire();
    }

    /**
     * Wake up at most one thread sleeping on this condition variable. The
     * current thread must hold the associated lock.
     */
    public void wake() {
        Lib.assertTrue(conditionLock.isHeldByCurrentThread());
        boolean intStatus = Machine.interrupt().disable();
        KThread next = waitQueue.nextThread();
        if (next != null) {
            next.ready();
        }
        Machine.interrupt().restore(intStatus);
    }

    /**
     * Wake up all threads sleeping on this condition variable. The current
     * thread must hold the associated lock.
     */
    public void wakeAll() {
        Lib.assertTrue(conditionLock.isHeldByCurrentThread());
        boolean intStatus = Machine.interrupt().disable();
        KThread next = waitQueue.nextThread();
        while (next != null) {
            next.ready();
            next = waitQueue.nextThread();
        }
        Machine.interrupt().restore(intStatus);
    }

    private Lock conditionLock;

    /*
     * We use the scheduler to create a ThreadQueue to keep track of all the 
     * threads waiting on this conditional variable.
     */ 
    private ThreadQueue waitQueue = ThreadedKernel.scheduler.newThreadQueue(false);

    /**
     * Tests the Condition2 class.
     */
    private static Lock testLock = new Lock();
    private static Condition2 testCondition = new Condition2(testLock);

    private static MustReachAssertion assert1a;
    private static MustReachAssertion assert1b;
    private static MustReachAssertion assert2a;
    private static MustReachAssertion assert2b;
    private static MustReachAssertion assert3a;
    private static MustReachAssertion assert3b;
    private static MustReachAssertion assert4a;
    private static MustReachAssertion assert4b;
    private static MustReachAssertion assert5a;
    private static MustReachAssertion assert5b;
    private static MustReachAssertion assert6a;
    private static MustReachAssertion assert6b;
    private static MustReachAssertion assert7a;
    private static MustReachAssertion assert7b;
    private static MustReachAssertion assert8a;
    private static MustReachAssertion assert8b;


    /**
     * This test only works for a scheduler that calls in order
     */
    public static void selfTest() {
        assert1a = TestStubs.getAssertReachedStart("Condition2 Test Case 1a");
        assert1b = TestStubs.getAssertReachedStart("Condition2 Test Case 1b");
        assert2a = TestStubs.getAssertReachedStart("Condition2 Test Case 2a");
        assert2b = TestStubs.getAssertReachedStart("Condition2 Test Case 2b");
        assert3a = TestStubs.getAssertReachedStart("Condition2 Test Case 3a");
        assert3b = TestStubs.getAssertReachedStart("Condition2 Test Case 3b");
        assert4a = TestStubs.getAssertReachedStart("Condition2 Test Case 4a");
        assert4b = TestStubs.getAssertReachedStart("Condition2 Test Case 4b");
        assert5a = TestStubs.getAssertReachedStart("Condition2 Test Case 5a");
        assert5b = TestStubs.getAssertReachedStart("Condition2 Test Case 5b");
        assert6a = TestStubs.getAssertReachedStart("Condition2 Test Case 6a");
        assert6b = TestStubs.getAssertReachedStart("Condition2 Test Case 6b");
        assert7a = TestStubs.getAssertReachedStart("Condition2 Test Case 7a");
        assert7b = TestStubs.getAssertReachedStart("Condition2 Test Case 7b");
        assert8a = TestStubs.getAssertReachedStart("Condition2 Test Case 8a");
        assert8b = TestStubs.getAssertReachedStart("Condition2 Test Case 8b");

        TestStubs.testStartPrint("Entering Condition2.selfTest");
        Runnable r1 = new Runnable() {
            public void run() {			
                testLock.acquire();
                assert1a.reached();
                TestStubs.debugPrint("Sleep() 1");
                testCondition.sleep();
                assert1b.reached();
                testLock.release();
            }
        };
        Runnable r2 = new Runnable() {
            public void run() {			
                testLock.acquire();
                assert2a.reached();
                TestStubs.debugPrint("Sleep() 2");
                testCondition.sleep();
                assert2b.reached();
                testLock.release();
            }
        };
        Runnable r3 = new Runnable() {
            public void run() {			
                testLock.acquire();
                assert3a.reached();
                TestStubs.debugPrint("Sleep() 3");
                testCondition.sleep();
                assert3b.reached();
                testLock.release();
            }
        };		
        Runnable r4 = new Runnable() {
            public void run() {			
                testLock.acquire();
                assert4a.reached();
                TestStubs.debugPrint("Wake() 4");
                testCondition.wake();
                assert4b.reached();
                testLock.release();
            }
        };		
        Runnable r5 = new Runnable() {
            public void run() {			
                testLock.acquire();
                assert5a.reached();
                TestStubs.debugPrint("WakeAll() 5");
                testCondition.wakeAll();
                assert5b.reached();
                testLock.release();
            }
        };
        Runnable r6 = new Runnable() {
            public void run() {			
                testLock.acquire();
                assert6a.reached();
                TestStubs.debugPrint("Sleep() 6");
                testCondition.sleep();
                assert6b.reached();
                testLock.release();
            }
        };
        Runnable r7 = new Runnable() {
            public void run() {			
                testLock.acquire();
                assert7a.reached();
                TestStubs.debugPrint("Sleep() 7");
                testCondition.sleep();
                assert7b.reached();
                testCondition.sleep();
                testLock.release();
            }
        };		
        Runnable r8 = new Runnable() {
            public void run() {			
                testLock.acquire();
                assert8a.reached();
                TestStubs.debugPrint("WakeAll() 8");
                testCondition.wakeAll();
                assert8b.reached();
                testLock.release();
            }
        };

        new KThread(r1).fork();
        new KThread(r2).fork();
        new KThread(r3).fork();
        new KThread(r4).fork();
        new KThread(r5).fork();	
        new KThread(r6).fork();
        new KThread(r7).fork();
        new KThread(r8).fork();	
        //Wait until all the threads have reached!
        while (!assert1a.hasReached() ||
                !assert1b.hasReached() ||
                !assert2a.hasReached() ||
                !assert2b.hasReached() ||
                !assert3a.hasReached() ||
                !assert3b.hasReached() ||
                !assert4a.hasReached() ||
                !assert4b.hasReached() ||
                !assert5a.hasReached() ||
                !assert5b.hasReached() ||
                !assert6a.hasReached() ||
                !assert6b.hasReached() ||
                !assert7a.hasReached() ||
                !assert7b.hasReached() ||
                !assert8a.hasReached() ||
                !assert8b.hasReached())
            KThread.yield();  
        TestStubs.testDonePrint("Finished Condition2.selfTest");
    }
}
