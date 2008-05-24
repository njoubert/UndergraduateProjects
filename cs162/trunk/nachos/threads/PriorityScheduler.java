package nachos.threads;

import nachos.machine.*;
import nachos.threads.TestStubs.MustReachAssertion;
import java.util.ArrayList;

/**
 * A scheduler that chooses threads based on their priorities.
 *
 * <p>
 * A priority scheduler associates a priority with each thread. The next thread
 * to be dequeued is always a thread with priority no less than any other
 * waiting thread's priority. Like a round-robin scheduler, the thread that is
 * dequeued is, among all the threads of the same (highest) priority, the
 * thread that has been waiting longest.
 *
 * <p>
 * Essentially, a priority scheduler gives access in a round-robin fassion to
 * all the highest-priority threads, and ignores all other threads. This has
 * the potential to
 * starve a thread if there's always a thread waiting with higher priority.
 *
 * <p>
 * A priority scheduler must partially solve the priority inversion problem; in
 * particular, priority must be donated through locks, and through joins.
 */
public class PriorityScheduler extends Scheduler {
    /**
     * Allocate a new priority scheduler.
     */
    public PriorityScheduler() {
        //Empty
    }

    /**
     * Allocate a new priority thread queue.
     *
     * @param	transferPriority	<tt>true</tt> if this queue should
     *					transfer priority from waiting threads
     *					to the owning thread.
     * @return	a new priority thread queue.
     */
    @Override
    public ThreadQueue newThreadQueue(boolean transferPriority) {
        return new PriorityQueue(transferPriority);
    }

    @Override
    public int getPriority(KThread thread) {
        Lib.assertTrue(Machine.interrupt().disabled());

        return getThreadState(thread).getPriority();
    }

    @Override
    public int getEffectivePriority(KThread thread) {
        Lib.assertTrue(Machine.interrupt().disabled());

        return getThreadState(thread).getEffectivePriority();
    }

    @Override
    public void setPriority(KThread thread, int priority) {
        Lib.assertTrue(Machine.interrupt().disabled());

        Lib.assertTrue(priority >= priorityMinimum &&
                priority <= priorityMaximum);

        getThreadState(thread).setPriority(priority);
    }

    @Override
    public boolean increasePriority() {
        boolean intStatus = Machine.interrupt().disable();

        KThread thread = KThread.currentThread();

        int priority = getPriority(thread);
        if (priority == priorityMaximum)
            return false;

        setPriority(thread, priority+1);

        Machine.interrupt().restore(intStatus);
        return true;
    }

    @Override
    public boolean decreasePriority() {
        boolean intStatus = Machine.interrupt().disable();

        KThread thread = KThread.currentThread();

        int priority = getPriority(thread);
        if (priority == priorityMinimum)
            return false;

        setPriority(thread, priority-1);

        Machine.interrupt().restore(intStatus);
        return true;
    }

    /**
     * The default priority for a new thread. Do not change this value.
     */
    public static final int priorityDefault = 1;
    /**
     * The minimum priority that a thread can have. Do not change this value.
     */
    public static final int priorityMinimum = 0;
    /**
     * The maximum priority that a thread can have. Do not change this value.
     */
    public static final int priorityMaximum = 7;    

    /**
     * Return the scheduling state of the specified thread.
     *
     * @param	thread	the thread whose scheduling state to return.
     * @return	the scheduling state of the specified thread.
     */
    protected ThreadState getThreadState(KThread thread) {
        if (thread.schedulingState == null)
            thread.schedulingState = new ThreadState(thread);

        return (ThreadState) thread.schedulingState;
    }

    /**
     * A <tt>ThreadQueue</tt> that sorts threads by priority.
     */
    protected class PriorityQueue extends ThreadQueue {
        PriorityQueue(boolean transferPriority) {
            this.transferPriority = transferPriority;
        }

        /**
         * @author Niels Joubert
         * This has the effect of donating the given thread's priority
         * to whoever owns the thread.
         */
        @Override
        public void waitForAccess(KThread thread) {
            Lib.assertTrue(Machine.interrupt().disabled());
            waiters.add(getThreadState(thread));
            recalculateDonation();
            getThreadState(thread).waitForAccess(this);
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
            recalculateDonation();
            acquire(next);

            if (next == null)
                return null;
            return next.thread;
        }

        /**
         * Acquires this resource for the current threadstate.
         * threadState can be null
         * @author Niels Joubert
         * @param threadState can be null
         */
        protected void acquire(ThreadState threadState) {
            if (threadState == null)
                owner = null;
            else
                acquire(threadState.thread);
        }
        /**
         * @author Niels Joubert
         * Has the effect of making the current thread the owner of this queue.
         * Priority donations will go up to this owner!
         */
        @Override
        public void acquire(KThread thread) {
            Lib.assertTrue(Machine.interrupt().disabled());
            getThreadState(thread).acquire(this);
            Lib.assertTrue(owner == null);
            owner = getThreadState(thread);
        }

        /**
         * Releases the current queue!
         * @author Niels Joubert
         * 
         */
        protected void release() {
            if (owner != null)
                owner.release(this);
            owner = null;
        }

        /**
         * Return the next thread that <tt>nextThread()</tt> would return,
         * without modifying the state of this queue.
         * 
         * RUNTIME: O(n) for n = size of waiters list
         *
         * @return	the next thread that <tt>nextThread()</tt> would
         *		return.
         */
        protected ThreadState pickNextThread() {
            //NIELS
            if (waiters.isEmpty())
                return null;
            ThreadState biggest = waiters.get(0);
            for (ThreadState waiter : waiters)
                if (biggest.getEffectivePriority() < waiter.getEffectivePriority())
                    biggest = waiter;
            return biggest;
        }

        @Override
        public void print() {
            Lib.assertTrue(Machine.interrupt().disabled());
            // implement me (if you want)
            if (owner != null)
                System.out.print("Owner = " + owner.thread.getName());
            System.out.print("Waiters = [");
            for (ThreadState waiter : waiters)
                System.out.print(waiter.thread.getName() + ", ");
            System.out.println("]");
        }

        //NIELS
        public int getDonation() {
            if (!transferPriority)
                return 0;
            return receivedPriority;
        }

        /**
         * Moves through the waiters list and calculates how much priority
         * is donated to this queue. Will send the new priority up to the owner
         * if the receivedPriority changes.
         */
        protected void recalculateDonation() {
            if (!transferPriority)
                return;

            int currentEP = getDonation();       

            int tempP = 0;
            for (ThreadState donator : waiters)
                tempP = Math.max(tempP, donator.getEffectivePriority());
            receivedPriority = Math.min(tempP, priorityMaximum);

            if (currentEP != getDonation() && owner != null) //if my effective priority changes:
                owner.pullDonationsUpToMe();

        }

        /**
         * <tt>true</tt> if this queue should transfer priority from waiting
         * threads to the owning thread.
         */
        public boolean transferPriority;
        /** The thread that owns whatever resource this queue protects. */
        protected ThreadState owner;
        /** The threads waiting on this queue. */
        protected ArrayList<ThreadState> waiters = new ArrayList<ThreadState>();;
        /** The priority received in donations from all the threads waiting on this queue. */
        protected int receivedPriority = 0;
    }

    /**
     * The scheduling state of a thread. This should include the thread's
     * priority, its effective priority, any objects it owns, and the queue
     * it's waiting for, if any.
     * 
     * PRIORITY DONATION:
     * on acquire: pull donations from all queues i own up to me.
     *
     * on release: remove what is released and again pull up donations 
     * from what i own
     *
     * on waitForAccess: get owner of queue to pull up donations. 
     * if owner's effective priority changes, get the queue its waiting for's 
     * owner to pull up donations.
     * 
     * on setPriority: get owner of queue its waiting for to pull up donations.
     * if owner's effective priority changes, get the queue its waiting for's
     * owner to pull up new donations.
     *
     * @see	nachos.threads.KThread#schedulingState
     */
    protected class ThreadState {
        /**
         * Allocate a new <tt>ThreadState</tt> object and associate it with the
         * specified thread.
         *
         * @param	thread	the thread this state belongs to.
         */
        public ThreadState(KThread thread) {
            this.thread = thread;

            setPriority(priorityDefault);
        }

        /**
         * Return the priority of the associated thread.
         *
         * RUNTIME: Constant Time
         *
         * @return	the priority of the associated thread.
         */
        public int getPriority() {
            return priority;
        }

        /**
         * Return the effective priority of the associated thread.
         * 
         * RUNTIME: Constant Time
         * 
         * @return	the effective priority of the associated thread.
         */
        public int getEffectivePriority() {
            return Math.max(receivedPriority, priority);
        }

        /**
         * Set the priority of the associated thread to the specified value.
         * 
         * RUNTIME: Must recalculate donation to owner - O(n) for 
         *              amount of queues 
         * 
         * 
         * @param	priority	the new priority.
         */
        public void setPriority(int priority) {
            if (this.priority == priority)
                return;

            this.priority = priority;

            if (waitFor != null)
                waitFor.recalculateDonation();

        }

        /**
         * Called when <tt>waitForAccess(thread)</tt> (where <tt>thread</tt> is
         * the associated thread) is invoked on the specified priority queue.
         * The associated thread is therefore waiting for access to the
         * resource guarded by <tt>waitQueue</tt>. This method is only called
         * if the associated thread cannot immediately obtain access.
         *
         * @param	waitQueue	the queue that the associated thread is
         *				now waiting on.
         *
         * RUNTIME:     Worst Case must traverse full tree upwards.
         *              Average Case: O(n) where n = amount of queues 
         *                      owned by the given queue's owner.
         *
         * @see	nachos.threads.ThreadQueue#waitForAccess
         */
        public void waitForAccess(PriorityQueue waitQueue) {
            Lib.assertTrue(waitFor == null);
            waitFor = waitQueue;
        }

        /**
         * Called when the associated thread has acquired access to whatever is
         * guarded by <tt>waitQueue</tt>. This can occur either as a result of
         * <tt>acquire(thread)</tt> being invoked on <tt>waitQueue</tt> (where
         * <tt>thread</tt> is the associated thread), or as a result of
         * <tt>nextThread()</tt> being invoked on <tt>waitQueue</tt>.
         *
         * RUNTIME: O(n) where n = amount of resources the thread owns
         *
         * @see	nachos.threads.ThreadQueue#acquire
         * @see	nachos.threads.ThreadQueue#nextThread
         */
        public void acquire(PriorityQueue waitQueue) {
            //NIELS
            Lib.assertTrue(waitQueue == waitFor || waitFor == null);
            waitFor = null;
            iOwn.add(waitQueue);
            Lib.assertTrue(waitFor == null);
        }

        protected void release(PriorityQueue waitQueue) {
            //NIELS
            iOwn.remove(waitQueue);
            pullDonationsUpToMe();
        }

        /**
         * Resets my receivedPriority by recalculating the 
         * priority donations from all
         * the resources i own.
         * 
         * Will send up donation to whatever im waiting on
         * if my effective priority changes.
         */
        protected void pullDonationsUpToMe() {
            //NIELS
            int tempP = 0;
            int currentEP = getEffectivePriority();

            for (PriorityQueue donator : iOwn)
                tempP = Math.max(tempP, donator.getDonation());
            receivedPriority = Math.min(tempP, priorityMaximum);

            if (getEffectivePriority() != currentEP && waitFor != null)
                sendDonationUpToWaitFor();
        }

        /**
         * Triggers the queue this thread is waiting on to recalculate
         * the donation it received from all the threads that waits on it, since
         * the donation of this thread changed.
         */
        protected void sendDonationUpToWaitFor() {
            if (waitFor != null)
                waitFor.recalculateDonation();
        }

        /** The thread with which this object is associated. */	   
        protected KThread thread;
        /** The priority of the associated thread. */
        protected int priority;
        /** The priority received in donations of the associated thread. */
        protected int receivedPriority = 0;

        //NIELS
        /** The Queue this thread is waiting to gain access to. Can be null*/
        protected PriorityQueue waitFor;
        /** Queues that I am the owner of. */
        protected ArrayList<PriorityQueue> iOwn = new ArrayList<PriorityQueue>();

    }

    public static void selfTest() {
        TestStubs.testStartPrint("Entering PriorityScheduler.selfTest"); 
        if (!(ThreadedKernel.scheduler instanceof PriorityScheduler) || (ThreadedKernel.scheduler instanceof LotteryScheduler) ) {
            TestStubs.debugPrint("PRIORITYSCHEDULER NOT ENABLED!");
        } else {
            assertt1 = TestStubs.getAssertReachedStart("PriorityQueue joinTest 1");
            assertt2 = TestStubs.getAssertReachedStart("PriorityQueue joinTest 2");
            testPriorityQueueAndThreadState();
            testSchedulerNoDonation();
            testDonationSimple();
            testMultipleWaitersOnQueue();
            testDonationOnJoin();
            testDonationOnLock();
        }
        TestStubs.testDonePrint("Finished PriorityScheduler.selfTest");
    }

    /**
     * - Tests Initial Blank Values
     * - Tests Adding One Thread to Queue
     * --- Tests Null Owner
     * --- Tests ThreadState's waitFor
     * --- Tests Queue's waiters
     * - Tests Acquiring Queue
     * --- Tests Queue's Owner
     * --- Tests Threadstate's iOwn
     * --- Tests unchanging effective priority
     * - Tests Waiting on Queue that has an owner
     * --- Tests unchanging owner
     * --- Tests ThreadState's waitFor
     * --- Tests Queue's waiters
     * - Tests Queue's nextThread
     * --- Tests old owner's waitFor and iOwn
     * --- Tests new owner's iOwn and waitFor
     * --- Tests queue's owner variable
     * --- Tests queue's waiters list
     */
    protected static void testPriorityQueueAndThreadState() {
        TestStubs.testStartPrint("Start PriorityQueue tests");
        PriorityScheduler.PriorityQueue queue1 = (PriorityScheduler.PriorityQueue) ThreadedKernel.scheduler.newThreadQueue(true);
        PriorityScheduler.PriorityQueue queue2 = (PriorityScheduler.PriorityQueue) ThreadedKernel.scheduler.newThreadQueue(true);

        Machine.interrupt().disable();

        TestStubs.debugPrint("Testing Initial Blank Values...");
        Lib.assertTrue(queue1.owner == null);
        Lib.assertTrue(queue1.waiters.isEmpty());

        TestStubs.debugPrint("Testing waiting on queue with no owner...");
        KThread ka = new KThread(new Runnable() { public void run() {/*stub*/}});
        queue1.waitForAccess(ka);
        Lib.assertTrue(queue1.owner == null);
        //Does the threadstate make the link?
        Lib.assertTrue(((ThreadState)ka.schedulingState).waitFor == queue1); 
        //Does the queue make the link?
        Lib.assertTrue(queue1.waiters.contains(ka.schedulingState));

        TestStubs.debugPrint("Testing acquiring queue...");
        KThread k1 = new KThread(new Runnable() { public void run() {/*stub*/}});
        queue2.acquire(k1);
        //Does the queue make the link?
        Lib.assertTrue(queue2.owner.thread == k1);
        //Does the threadstate make the link?
        Lib.assertTrue(((ThreadState)k1.schedulingState).iOwn.contains(queue2));
        //Does the priority not get affected as it should?
        Lib.assertTrue(((ThreadState)k1.schedulingState).getEffectivePriority() == 
            ((ThreadState)k1.schedulingState).getPriority());


        TestStubs.debugPrint("Testing waiting on queue that has owner...");
        KThread k2 = new KThread(new Runnable() { public void run() {/*stub*/}});
        queue2.waitForAccess(k2);
        Lib.assertTrue(((ThreadState)k1.schedulingState).iOwn.contains(queue2)); 
        Lib.assertTrue(queue2.owner.thread == k1);
        //Does the threadstate make the link?
        Lib.assertTrue(((ThreadState)k2.schedulingState).waitFor == queue2); 
        //Does the queue make the link?
        Lib.assertTrue(queue2.waiters.contains(k2.schedulingState));

        TestStubs.debugPrint("Testing nextThread() on queue...");
        queue2.nextThread();
        //Does the k1 threadstate make the link?
        Lib.assertTrue(((ThreadState)k1.schedulingState).waitFor == null);
        Lib.assertTrue(((ThreadState)k1.schedulingState).iOwn.size() == 1);
        //Does the k2 threadstate make the link?
        Lib.assertTrue(((ThreadState)k2.schedulingState).iOwn.contains(queue2)); 
        Lib.assertTrue(((ThreadState)k2.schedulingState).waitFor == null); 
        //Does the queue make the link?
        Lib.assertTrue(queue2.owner.thread == k2);
        Lib.assertTrue(queue2.waiters.isEmpty());

        Machine.interrupt().enable();
        TestStubs.testDonePrint("Finished PriorityQueue tests");

    }
    private static void testSchedulerNoDonation() {
        TestStubs.testStartPrint("Start Scheeduler no donarion tests");
        Machine.interrupt().disable();

        PriorityScheduler.PriorityQueue q1 = (PriorityScheduler.PriorityQueue) ThreadedKernel.scheduler.newThreadQueue(false);
        Runnable r = new Runnable() { public void run() {/*stub*/}};
        KThread k1 = new KThread(r);
        KThread k2 = new KThread(r);

        q1.waitForAccess(k1);
        q1.waitForAccess(k2);
        ThreadedKernel.scheduler.setPriority(k1, 1);
        Lib.assertTrue(q1.pickNextThread() == k1.schedulingState);
        ThreadedKernel.scheduler.setPriority(k2, 1);
        Lib.assertTrue(q1.pickNextThread() == k1.schedulingState);
        ThreadedKernel.scheduler.setPriority(k1, 2);
        Lib.assertTrue(q1.pickNextThread() == k1.schedulingState);

        PriorityScheduler.PriorityQueue q2 = (PriorityScheduler.PriorityQueue) ThreadedKernel.scheduler.newThreadQueue(false);

        KThread r1 = new KThread(new Runnable() {
            public void run() {

            }
        });
        KThread r2 = new KThread(new Runnable() {
            public void run() {

            }
        });
        ThreadedKernel.scheduler.setPriority(r1, 2);
        ThreadedKernel.scheduler.setPriority(r2, 0);
        Lib.assertTrue(((ThreadState)r1.schedulingState).getEffectivePriority() == 2);
        Lib.assertTrue(((ThreadState)r2.schedulingState).getEffectivePriority() == 0);
        Machine.interrupt().enable();
        TestStubs.testDonePrint("Finished Scheeduler no donarion tests");
    }
    /**
     * - Tests Setting Up Thread Priorities
     * - Tests equality of Effective and Normal priorities under initial setup
     * - Tests high priority thread waiting on queue with low priority owner
     * --- Tests effective priority versus real priority of owner
     * --- Tests high priority's priority invariance
     * - Tests nextThread() changing priorities
     * --- Tests old owner losing donated priority
     * --- Tests old owner's real priority invariance
     * --- Tests new owner's unchanging effective priority (no waiters)
     */
    private static void testDonationSimple() {
        TestStubs.testStartPrint("Start Simple Donation tests");
        Machine.interrupt().disable();

        PriorityScheduler.PriorityQueue queue = (PriorityScheduler.PriorityQueue) ThreadedKernel.scheduler.newThreadQueue(true);
        Runnable r = new Runnable() { public void run() {/*stub*/}};
        KThread k1 = new KThread(r);
        KThread k2 = new KThread(r);

        TestStubs.debugPrint("Created two threads with different priorities.");
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

        //Did k1, the low priority thread, get the priority of k2?
        Lib.assertTrue(((ThreadState)k1.schedulingState).getEffectivePriority()
                == ((ThreadState)k2.schedulingState).getPriority());
        //Did k1's normal priority stay the same?
        Lib.assertTrue(((ThreadState)k1.schedulingState).getPriority() == k1initialp);

        //boost up k2, does k1 also go up?
        ThreadedKernel.scheduler.setPriority(k2, 6);
        Lib.assertTrue(((ThreadState)k1.schedulingState).getEffectivePriority()
                == ((ThreadState)k2.schedulingState).getPriority());
        ThreadedKernel.scheduler.setPriority(k2, 4);
        Lib.assertTrue(((ThreadState)k1.schedulingState).getEffectivePriority()
                == ((ThreadState)k2.schedulingState).getPriority());
        ThreadedKernel.scheduler.setPriority(k2, k1initialp);
        Lib.assertTrue(((ThreadState)k1.schedulingState).getEffectivePriority()
                == ((ThreadState)k2.schedulingState).getPriority());

        TestStubs.debugPrint("calling nextThread()... k2 now the owner");
        queue.nextThread();

        printThreadPriority("   k1", k1);
        printThreadPriority("   k2", k2);

        //Did k1, the low priority thread, return to its old priority?
        Lib.assertTrue(((ThreadState)k1.schedulingState).getEffectivePriority()
                == ((ThreadState)k1.schedulingState).getPriority());
        //Did k1's normal priority stay the same?
        Lib.assertTrue(((ThreadState)k1.schedulingState).getPriority() == k1initialp);
        //Did k2's priority stay the same?
        Lib.assertTrue(((ThreadState)k2.schedulingState).getEffectivePriority()
                == ((ThreadState)k2.schedulingState).getPriority());

        Machine.interrupt().enable();
        TestStubs.testDonePrint("Finished Simple Donation tests");
    }
    /**
     * - Tests Correct Effective Priority with multiple threads on queue
     * - Tests Correct nextThread() for ALL 5 THREADS ON QUEUE
     * --- Tests highest priority next thread
     * --- Tests oldest thread with same priority first
     * - Tests Correct order of nextThread() for 4 equal-priority threads
     */
    private static void testMultipleWaitersOnQueue() {
        TestStubs.testStartPrint("Start Donation through Lock tests");
        Machine.interrupt().disable();
        PriorityScheduler.PriorityQueue q = (PriorityScheduler.PriorityQueue) ThreadedKernel.scheduler.newThreadQueue(true);
        Runnable r = new Runnable() { public void run() {/*stub*/}};
        KThread k1 = new KThread(r);
        KThread k2 = new KThread(r);
        KThread k3 = new KThread(r);
        KThread k4 = new KThread(r);
        KThread k5a = new KThread(r);
        KThread k5b = new KThread(r);
        TestStubs.debugPrint("Created 5 threads with different priorities.");
        ThreadedKernel.scheduler.setPriority(k1, 1);
        ThreadedKernel.scheduler.setPriority(k2, 2);
        ThreadedKernel.scheduler.setPriority(k3, 3);
        ThreadedKernel.scheduler.setPriority(k4, 4);
        ThreadedKernel.scheduler.setPriority(k5a, 5);
        ThreadedKernel.scheduler.setPriority(k5b, 5);
        int k1initialp = ((ThreadState)k1.schedulingState).getPriority();
        printThreadPriority("   k1", k1);
        printThreadPriority("   k2", k2);
        printThreadPriority("   k3", k3);
        printThreadPriority("   k4", k4);
        printThreadPriority("   k5a", k5a);
        printThreadPriority("   k5b", k5b);

        TestStubs.debugPrint("K1 (lowest priority) thread Acquiring Queue...");
        q.acquire(k1);
        Lib.assertTrue(((ThreadState)k1.schedulingState).iOwn.contains(q)); 
        Lib.assertTrue(q.owner.thread == k1);
        TestStubs.debugPrint("K2 though K5a/b (highest priority) threads waiting on queue...");
        q.waitForAccess(k4);
        q.waitForAccess(k2);
        q.waitForAccess(k3);
        q.waitForAccess(k5a);
        q.waitForAccess(k5b);
        printThreadPriority("   k1[owner]", k1);
        printThreadPriority("   k2", k2);
        printThreadPriority("   k3", k3);
        printThreadPriority("   k4", k4);
        printThreadPriority("   k5a", k5a); //Added first
        printThreadPriority("   k5b", k5b); //Added Second

        //Did the owner get the highest priorty thread's priority?
        Lib.assertTrue(((ThreadState)k1.schedulingState).getEffectivePriority()
                == ((ThreadState)k5a.schedulingState).getPriority());
        //Did the rest stay the same?
        Lib.assertTrue(((ThreadState)k2.schedulingState).getEffectivePriority()
                == ((ThreadState)k2.schedulingState).getPriority());
        Lib.assertTrue(((ThreadState)k3.schedulingState).getEffectivePriority()
                == ((ThreadState)k3.schedulingState).getPriority());
        Lib.assertTrue(((ThreadState)k4.schedulingState).getEffectivePriority()
                == ((ThreadState)k4.schedulingState).getPriority());
        Lib.assertTrue(((ThreadState)k5a.schedulingState).getEffectivePriority()
                == ((ThreadState)k5a.schedulingState).getPriority());
        Lib.assertTrue(((ThreadState)k5b.schedulingState).getEffectivePriority()
                == ((ThreadState)k5b.schedulingState).getPriority());


        //k5a must be the new owner after calling nextThread()
        TestStubs.debugPrint("calling nextThread()... k5a must be owner");
        q.nextThread();
        Lib.assertTrue(((ThreadState)k5a.schedulingState).iOwn.contains(q));
        Lib.assertTrue(!((ThreadState)k1.schedulingState).iOwn.contains(q));
        Lib.assertTrue(q.owner.thread == k5a);
        Lib.assertTrue(q.waiters.contains(k5b.schedulingState));
        Lib.assertTrue(q.waiters.contains(k4.schedulingState));
        Lib.assertTrue(q.waiters.contains(k3.schedulingState));
        Lib.assertTrue(q.waiters.contains(k2.schedulingState));
        //Did the owner get the highest priorty thread's priority?
        Lib.assertTrue(((ThreadState)k1.schedulingState).getEffectivePriority()
                == ((ThreadState)k1.schedulingState).getPriority());
        Lib.assertTrue(((ThreadState)k5a.schedulingState).getEffectivePriority()
                == ((ThreadState)k5a.schedulingState).getPriority());

        //k5b must be the new owner after calling nextThread()
        TestStubs.debugPrint("calling nextThread()... k5b must be owner");
        q.nextThread();
        Lib.assertTrue(((ThreadState)k5b.schedulingState).iOwn.contains(q)); 
        Lib.assertTrue(!((ThreadState)k1.schedulingState).iOwn.contains(q));
        Lib.assertTrue(!((ThreadState)k5a.schedulingState).iOwn.contains(q));
        Lib.assertTrue(q.owner.thread == k5b);
        Lib.assertTrue(q.waiters.contains(k4.schedulingState));
        Lib.assertTrue(q.waiters.contains(k3.schedulingState));
        Lib.assertTrue(q.waiters.contains(k2.schedulingState));

        //k4 must be the new owner after calling nextThread()
        TestStubs.debugPrint("calling nextThread()... k4 must be owner");
        q.nextThread();
        Lib.assertTrue(((ThreadState)k4.schedulingState).iOwn.contains(q)); 
        Lib.assertTrue(!((ThreadState)k1.schedulingState).iOwn.contains(q));
        Lib.assertTrue(!((ThreadState)k5a.schedulingState).iOwn.contains(q));
        Lib.assertTrue(!((ThreadState)k5b.schedulingState).iOwn.contains(q));
        Lib.assertTrue(q.owner.thread == k4);
        Lib.assertTrue(q.waiters.contains(k3.schedulingState));
        Lib.assertTrue(q.waiters.contains(k2.schedulingState));

        //k3 must be the new owner after calling nextThread()
        TestStubs.debugPrint("calling nextThread()... k3 must be owner");
        q.nextThread();
        Lib.assertTrue(((ThreadState)k3.schedulingState).iOwn.contains(q)); 
        Lib.assertTrue(!((ThreadState)k1.schedulingState).iOwn.contains(q));
        Lib.assertTrue(!((ThreadState)k5a.schedulingState).iOwn.contains(q));
        Lib.assertTrue(!((ThreadState)k5b.schedulingState).iOwn.contains(q));
        Lib.assertTrue(!((ThreadState)k4.schedulingState).iOwn.contains(q));
        Lib.assertTrue(q.owner.thread == k3);
        Lib.assertTrue(q.waiters.contains(k2.schedulingState));

        //k2 must be the new owner after calling nextThread()
        TestStubs.debugPrint("calling nextThread()... k2 must be owner");
        q.nextThread();
        Lib.assertTrue(((ThreadState)k2.schedulingState).iOwn.contains(q)); 
        Lib.assertTrue(!((ThreadState)k1.schedulingState).iOwn.contains(q));
        Lib.assertTrue(!((ThreadState)k5a.schedulingState).iOwn.contains(q));
        Lib.assertTrue(!((ThreadState)k5b.schedulingState).iOwn.contains(q));
        Lib.assertTrue(!((ThreadState)k4.schedulingState).iOwn.contains(q));
        Lib.assertTrue(!((ThreadState)k3.schedulingState).iOwn.contains(q));
        Lib.assertTrue(q.owner.thread == k2);

        //no-one must be the new owner after calling nextThread()
        TestStubs.debugPrint("calling nextThread()... no owner!");
        q.nextThread();
        Lib.assertTrue(((ThreadState)k1.schedulingState).iOwn.size() == 1); 
        Lib.assertTrue(q.owner == null);

        //Testing order of equal-priority threads to return
        KThread k6a = new KThread(r);
        KThread k6b = new KThread(r);
        KThread k6c = new KThread(r);
        KThread k6d = new KThread(r);
        ThreadedKernel.scheduler.setPriority(k6a, 6);
        ThreadedKernel.scheduler.setPriority(k6b, 6);
        ThreadedKernel.scheduler.setPriority(k6c, 6);
        ThreadedKernel.scheduler.setPriority(k6d, 6);
        q.waitForAccess(k6a);
        q.waitForAccess(k6b);
        q.waitForAccess(k6c);
        q.waitForAccess(k6d);

        q.nextThread();
        Lib.assertTrue(q.owner.thread == k6a);
        q.nextThread();
        Lib.assertTrue(q.owner.thread == k6b);
        q.nextThread();
        Lib.assertTrue(q.owner.thread == k6c);
        q.nextThread();
        Lib.assertTrue(q.owner.thread == k6d);

        //Did the initial priorities stay the same?
        Lib.assertTrue(((ThreadState)k1.schedulingState).getEffectivePriority()
                == k1initialp);

        Machine.interrupt().enable();
        TestStubs.testDonePrint("Finished Donation through Lock tests");
    }

    private static KThread k1, k2;
    static MustReachAssertion assertt1;
    static MustReachAssertion assertt2;


    /**
     * - Tests Correct Effective Priority with joining one thread on another.
     * - Tests that the highest priority thread gets run first!
     */
    private static void testDonationOnJoin() {
        TestStubs.testStartPrint("Start Donation through Join tests");
        int k1init = 2;
        int k2init = 7;

        Machine.interrupt().disable();

        k1 = new KThread(new Runnable() { public void run() {
            TestStubs.debugPrint("k1 (lower priority) running!");
            Lib.assertTrue(!assertt1.hasReached());
            Lib.assertTrue(assertt2.hasReached());


            //Check join queue!

            //Check my priority!
            printThreadPriority("   k1", k1);
            printThreadPriority("   k2", k2);
            Lib.assertTrue(((ThreadState)k1.schedulingState).getEffectivePriority()
                    == ((ThreadState)k2.schedulingState).getPriority());

            assertt1.reached();
        }});
        k2 = new KThread(new Runnable() { public void run() {
            TestStubs.debugPrint("k2 (high priority) running! Joining to k1...");
            Lib.assertTrue(!assertt1.hasReached());
            Lib.assertTrue(!assertt2.hasReached());
            assertt2.reached();
            k1.join();

            TestStubs.debugPrint("k2 (high priority) returned from join!");
            printThreadPriority("   k1", k1);
            printThreadPriority("   k2", k2);
            //Check that my priority is still my initial value
            Lib.assertTrue(((ThreadState)k2.schedulingState).getEffectivePriority()
                    == ((ThreadState)k2.schedulingState).getPriority());
        }});
        TestStubs.debugPrint("Setting up k1 and k2 priorities...");
        ThreadedKernel.scheduler.setPriority(k1, k1init);
        ThreadedKernel.scheduler.setPriority(k2, k2init);
        k1.fork();
        k2.fork();
        Lib.assertTrue(!assertt1.hasReached() || !assertt2.hasReached());
        KThread.yield(); 

        //Since the main thread has lowest priority, these should both me done by now!
        Lib.assertTrue(assertt1.hasReached() && assertt2.hasReached());

        Machine.interrupt().disable();
        TestStubs.testDonePrint("Finished Donation through Join tests");
    }
    /**
     * - Tests Correct Effective Priority through locks
     */
    private static void testDonationOnLock() {
        TestStubs.testStartPrint("Start Donation through Lock tests");

        //thread 1 acquires lock (highest priority)
        //thread 2 attempts to acquire lock
        //thread 1 must not have a higher priority


        TestStubs.testDonePrint("Finished Donation through Lock tests");
    }

    protected static void printThreadPriority(String name, KThread thread) {
        TestStubs.debugPrint(name+"(p=" 
                + ((ThreadState)thread.schedulingState).getPriority()
                + ", Ep="
                + ((ThreadState)thread.schedulingState).getEffectivePriority()
                + ")");
    }
}
