package nachos.threads;

import java.util.ArrayList;

import nachos.machine.*;
import nachos.threads.PriorityScheduler.PriorityQueue;
import nachos.threads.PriorityScheduler.ThreadState;
import nachos.threads.TestStubs.MustReachAssertion;


/**
 * A scheduler that chooses threads using a lottery.
 *
 * <p>
 * A lottery scheduler associates a number of tickets with each thread. When a
 * thread needs to be dequeued, a random lottery is held, among all the tickets
 * of all the threads waiting to be dequeued. The thread that holds the winning
 * ticket is chosen.
 *
 * <p>
 * Note that a lottery scheduler must be able to handle a lot of tickets
 * (sometimes billions), so it is not acceptable to maintain state for every
 * ticket.
 *
 * <p>
 * A lottery scheduler must partially solve the priority inversion problem; in
 * particular, tickets must be transferred through locks, and through joins.
 * Unlike a priority scheduler, these tickets add (as opposed to just taking
 * the maximum).
 */
public class LotteryScheduler extends PriorityScheduler {
    /**
     * Allocate a new lottery scheduler.
     */
    public LotteryScheduler() {
        //empty
    }
    
    /**
     * Allocate a new lottery thread queue.
     *
     * @param	transferPriority	<tt>true</tt> if this queue should
     *					transfer tickets from waiting threads
     *					to the owning thread.
     * @return	a new lottery thread queue.
     */
    @Override
    public ThreadQueue newThreadQueue(boolean transferPriority) {
	return new LotteryQueue(transferPriority);
    }
    
    /**
     * Return the scheduling state of the specified thread.
     *
     * @param   thread  the thread whose scheduling state to return.
     * @return  the scheduling state of the specified thread.
     */
    @Override
    protected ThreadState getThreadState(KThread thread) {
        if (thread.schedulingState == null)
            thread.schedulingState = new ThreadState(thread);

        return (ThreadState) thread.schedulingState;
    }
    
    public static final int priorityDefault = 1;
    public static final int priorityMinimum = 1;
    public static final int priorityMaximum = Integer.MAX_VALUE; 
    
    protected class LotteryQueue extends PriorityQueue {

        LotteryQueue(boolean transferPriority) {
            super(transferPriority);
        }
        
        /**
         * Changes the state of the current queue by releasing the current
         * owner and picking the next owner.
         * 
         * RUNTIME: O(n) for n = size of waiters list
         */
        @Override
        public KThread nextThread() {
            Lib.assertTrue(Machine.interrupt().disabled());
            //NIELS
            release();
            ThreadState next = pickNextThread();
            waiters.remove(next);
            acquire(next);
            recalculateDonation();

            if (next == null)
                return null;
            return next.thread;
        }
        
        @Override
        protected void recalculateDonation() {
            //TestStubs.debugPrint("recalc!!!!!!!!!!");
            if (!transferPriority)
                return;

            if (isthereaLoop()) {
                throw new RuntimeException("FUCK you NIGGA! You have a loop in your code");
            }
            
            int currentEP = getDonation();       

            int tempP = 0;
            for (PriorityScheduler.ThreadState donator : waiters)
                tempP += donator.getEffectivePriority();
            receivedPriority = tempP;

            if (currentEP != getDonation() && owner != null) //if my effective priority changes:
                owner.pullDonationsUpToMe();
        }
        
        private boolean isthereaLoop() {
//            ThreadQueue curr;
//            while (owner != null && owner.waitFor != null) {
//                curr = owner.waitFor;
//                if (curr == this)
//                    return true;
//            }   
            return false;
        }
        

        /**
         * Randomly picks the next thread off the queue, with the
         * probability of a thread being picked proportional
         * to the amount of tickets that thread owns.
         * 
         * RUNTIME: O(n)
         */
        @Override
        protected ThreadState pickNextThread() {
            if (waiters.isEmpty())
                return null;
            int sum = 0, i = 0;
            for (PriorityScheduler.ThreadState waiter : waiters)
                sum += waiter.getEffectivePriority();
            int random = (int) (sum * Lib.random());
            sum = 0;
            i = 0;
            for (; i < waiters.size(); i++) {
                sum += waiters.get(i).getEffectivePriority();
                if (random < sum)
                    return (nachos.threads.LotteryScheduler.ThreadState) waiters.get(i);
            }
            return (nachos.threads.LotteryScheduler.ThreadState) waiters.get(waiters.size());
   
        }
        
    }
    
    protected class ThreadState extends PriorityScheduler.ThreadState {

        @Override
        public int getEffectivePriority() {
            return (receivedPriority + priority);
        }
        
        public ThreadState(KThread thread) {
            super(thread);
        }
        
        @Override
        protected void pullDonationsUpToMe() {
            int tempP = 0;
            int currentEP = getEffectivePriority();

            for (PriorityQueue donator : iOwn)
                tempP += donator.getDonation();
            receivedPriority = tempP; //

            if (getEffectivePriority() != currentEP)
                sendDonationUpToWaitFor();
        }
        
    }
    
    public static void selfTest() {
        TestStubs.testStartPrint("Entering LotterScheduler.selfTest");
        if (!(ThreadedKernel.scheduler instanceof LotteryScheduler))
            TestStubs.debugPrint("LOTTERYSCHEDULER NOT ENABLED!");
        else {
            // Run a bunch of tests here
            testPriorityQueueAndThreadState();
            testDonationSimple();
        }
                
    }
    
    
    private static void testDonationSimple() {
        TestStubs.testStartPrint("Start Simple Ticket Donation tests");
        Machine.interrupt().disable();

        PriorityScheduler.PriorityQueue queue = (PriorityScheduler.PriorityQueue) ThreadedKernel.scheduler.newThreadQueue(true);
        Runnable r = new Runnable() { public void run() {/*stub*/}};
        KThread k1 = new KThread(r);
        KThread k2 = new KThread(r);

        TestStubs.debugPrint("Created two threads with different tickets.");
        ThreadedKernel.scheduler.setPriority(k1, 2);
        ThreadedKernel.scheduler.setPriority(k2, 5);
        printThreadPriority("   k1", k1);
        printThreadPriority("   k2", k2);

        int k1initialp = ((ThreadState)k1.schedulingState).getPriority();

        TestStubs.debugPrint("K1 (low priority) thread Acquiring Queue...");
        queue.acquire(k1);
        TestStubs.debugPrint("K2 (high priority) thread waiting on queue...");
        queue.waitForAccess(k2);

        printThreadPriority("   k1", k1);
        printThreadPriority("   k2", k2);

        // K1 should now have K1+K2 tickets
        Lib.assertTrue(((ThreadState)k1.schedulingState).getEffectivePriority()
                == ((ThreadState)k2.schedulingState).getPriority() + ((ThreadState)k1.schedulingState).getPriority());
        
        KThread k3 = new KThread(r);
        ThreadedKernel.scheduler.setPriority(k3, 1);
        PriorityScheduler.PriorityQueue queue2 = (PriorityScheduler.PriorityQueue) ThreadedKernel.scheduler.newThreadQueue(true);
        queue2.acquire(k3);
        TestStubs.debugPrint("k3 acquired queue2...");
        
        printThreadPriority("   k3", k3);
        queue.waitForAccess(k3);
        TestStubs.debugPrint("K3 thread waiting on queue...");
        
        printThreadPriority("   k1", k1);
        printThreadPriority("   k2", k2);
        printThreadPriority("   k3", k3);  
        
        TestStubs.debugPrint("asdf");
        
        queue2.waitForAccess(k1);
        TestStubs.debugPrint("K1 waiting on queue2...");
        
        printThreadPriority("   k1", k1);
        printThreadPriority("   k2", k2);
        printThreadPriority("   k3", k3); 
        
   
        
        Machine.interrupt().enable();
    }
    
    
    
    static MustReachAssertion assertt1;
}

