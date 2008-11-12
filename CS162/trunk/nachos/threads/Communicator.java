package nachos.threads;

import nachos.threads.TestStubs.MustReachAssertion;

import java.util.LinkedList;

/**
 * A <i>communicator</i> allows threads to synchronously exchange 32-bit
 * messages. Multiple threads can be waiting to <i>speak</i>,
 * and multiple threads can be waiting to <i>listen</i>. But there should never
 * be a time when both a speaker and a listener are waiting, because the two
 * threads can be paired off at this point.
 */
public class Communicator {
	private Lock commLock;
	private LinkedList<Integer> buffer;
	private Condition2 canSpeak, canListen, hasRead;
	private boolean hasBeenRead;
    /**
     * Allocate a new communicator.
     */
	
    public Communicator() {
    	this.commLock = new Lock();
    	this.buffer = new LinkedList<Integer>();
    	this.canSpeak = new Condition2(commLock);
    	this.canListen = new Condition2(commLock);
    	this.hasRead = new Condition2(commLock);
    	this.hasBeenRead = false;
    }

    /**
     * Wait for a thread to listen through this communicator, and then transfer
     * <i>word</i> to the listener.
     *
     * <p>
     * Does not return until this thread is paired up with a listening thread.
     * Exactly one listener should receive <i>word</i>.
     *
     * @param	word	the integer to transfer.
     */

    public void speak(int word) {
    	this.commLock.acquire();
    	while (!this.buffer.isEmpty())
    		this.canSpeak.sleep();
    	this.buffer.add(word);
    	this.canListen.wake();
    	while (!this.hasBeenRead)
    		this.hasRead.sleep();
    	this.hasBeenRead = false;
    	this.canSpeak.wake();
    	this.commLock.release();
    }

    /**
     * Wait for a thread to speak through this communicator, and then return
     * the <i>word</i> that thread passed to <tt>speak()</tt>.
     *
     * @return	the integer transferred.
     */    
    public int listen() {
    	this.commLock.acquire();
    	while (this.buffer.isEmpty())
    		this.canListen.sleep();
    	int ret = (int) this.buffer.removeFirst();
    	this.hasBeenRead = true;
    	this.hasRead.wake();
    	this.commLock.release();
    	return ret;
    }

    /** Testing Portion of the Communicator class */
    private static Communicator comm1 = new Communicator();
    private static Communicator comm2 = new Communicator();
    private static Communicator comm3 = new Communicator();
    private static Communicator comm4 = new Communicator();
    private static Communicator comm5 = new Communicator();
    private static Communicator comm6 = new Communicator();    
    private static MustReachAssertion assertt1a = TestStubs.getAssertReachedStart("Communicator Test 1a");
    private static MustReachAssertion assertt1b = TestStubs.getAssertReachedStart("Communicator Test 1b");
    private static MustReachAssertion assertt2a = TestStubs.getAssertReachedStart("Communicator Test 2a");
    private static MustReachAssertion assertt2b = TestStubs.getAssertReachedStart("Communicator Test 2b");
    private static MustReachAssertion assertt3a = TestStubs.getAssertReachedStart("Communicator Test 3a");
    private static MustReachAssertion assertt3b = TestStubs.getAssertReachedStart("Communicator Test 3b");
    private static MustReachAssertion assertt4a = TestStubs.getAssertReachedStart("Communicator Test 4a");
    private static MustReachAssertion assertt4b = TestStubs.getAssertReachedStart("Communicator Test 4b");
    private static MustReachAssertion assertt5a = TestStubs.getAssertReachedStart("Communicator Test 5a");
    private static MustReachAssertion assertt5b = TestStubs.getAssertReachedStart("Communicator Test 5b");
    private static MustReachAssertion assertt6a = TestStubs.getAssertReachedStart("Communicator Test 6a");
    private static MustReachAssertion assertt6b = TestStubs.getAssertReachedStart("Communicator Test 6b");
    private static MustReachAssertion assertt7a = TestStubs.getAssertReachedStart("Communicator Test 7a");
    private static MustReachAssertion assertt7b = TestStubs.getAssertReachedStart("Communicator Test 7b");
    
    /**
     * Tests the Communicator class.
     * Please see the Design Doc for Project 1
     */
    public static void selfTest() {
    	TestStubs.testStartPrint("Entering Communicator.selfTest");
    	
    	Runnable r1 = new Runnable() {
    		public void run() {			
    			comm1.speak(1);
    			assertt1a.reached();
    		}
    	};
    	Runnable r2 = new Runnable() {
    		public void run() {
    		TestStubs.debugPrint("One Speaker, One Listener: " + comm1.listen());
    			assertt1b.reached();
    		}
    	};
    	Runnable r3 = new Runnable() {
    		public void run() {			
    		TestStubs.debugPrint("One Listener, One Speaker: " + comm2.listen());
    			assertt2a.reached();
    		}
    	};
    	Runnable r4 = new Runnable() {
    		public void run() {			
    			comm2.speak(2);
    			assertt2b.reached();
    		}
    	};
    	Runnable r5 = new Runnable() {
    		public void run() {			
    		TestStubs.debugPrint("Multiple Listeners, One Speaker (1): " + comm3.listen());
    			assertt3a.reached();
    			TestStubs.debugPrint("Multiple Listeners, One Speaker (2): " + comm3.listen());
    		}
    	};
    	Runnable r6 = new Runnable() {
    		public void run() {			
    			comm3.speak(3);
    			assertt3b.reached();
    		}
    	};
    	Runnable r7 = new Runnable() {
    		public void run() {			
    			comm4.speak(4);
    			assertt4a.reached();
    		}
    	};
    	Runnable r8 = new Runnable() {
    		public void run() {
    		TestStubs.debugPrint("Multiple Speakers, One Listener (1): " + comm4.listen());
    	    	assertt4b.reached();
    	    	TestStubs.debugPrint("Multiple Speakers, One Listener (2): " + comm4.listen());
    		}
    	};
    	Runnable r9 = new Runnable() {
    		public void run() {			
    			comm5.speak(5);
    			comm5.speak(6);
    			assertt5a.reached();
    		}
    	};
    	Runnable r10 = new Runnable() {
    		public void run() {
    		TestStubs.debugPrint("Multiple Speakers, Multiple Listeners (1): " + comm5.listen());
    		TestStubs.debugPrint("Multiple Speakers, Multiple Listeners (2): " + comm5.listen());
    			assertt5b.reached();
    		}
    	};    	
    	Runnable r11 = new Runnable() {
    		public void run() {			
    			comm6.speak(7);
    			comm6.speak(8);
    			comm6.speak(9);
    			assertt6a.reached();
    		}
    	};
    	Runnable r12 = new Runnable() {
    		public void run() {
    			TestStubs.debugPrint("Multiple Speakers, Multiple Listeners [interleaved] (1): " + comm6.listen());   
    			assertt6b.reached();
    		}
    	}; 
    	Runnable r13 = new Runnable() {
    		public void run() {
    		        TestStubs.debugPrint("Multiple Speakers, Multiple Listeners [interleaved] (2): " + comm6.listen());   
    			assertt7a.reached();
    		}
    	}; 
    	Runnable r14 = new Runnable() {
    		public void run() {
    		        TestStubs.debugPrint("Multiple Speakers, Multiple Listeners [interleaved] (3): " + comm6.listen());   
    			assertt7b.reached();
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
    	new KThread(r9).fork();
    	new KThread(r10).fork();
    	new KThread(r11).fork();
    	new KThread(r12).fork();
    	new KThread(r13).fork();
    	new KThread(r14).fork();
    	
    	//Wait until all the threads have reached!
    	while (!assertt1a.hasReached() ||
    			!assertt1b.hasReached() ||
    			!assertt2a.hasReached() ||
    			!assertt2b.hasReached() ||
    			!assertt3a.hasReached() ||
    			!assertt3b.hasReached() ||
    			!assertt4a.hasReached() ||
    			!assertt4b.hasReached() ||
    			!assertt5a.hasReached() || 
    			!assertt5b.hasReached() ||
    			!assertt6a.hasReached() ||
    			!assertt6b.hasReached() ||
    			!assertt7a.hasReached() ||
    			!assertt7b.hasReached())
    		KThread.yield();  
    	TestStubs.testDonePrint("Finished Communicator.selfTest");
    }

}
