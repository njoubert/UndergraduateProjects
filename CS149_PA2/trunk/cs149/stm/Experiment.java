package cs149.stm;

import java.util.Random;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class Experiment implements Runnable {

    private final int _numThreads;
    private final int _keyRange;
    private final int _readPct;
    private final IntSet _impl;
    private final int _millis;

    private volatile boolean _done;

    public Experiment(final int numThreads,
                      final int keyRange,
                      final int readPct,
                      final IntSet impl,
                      final int millis) {
        _numThreads = numThreads;
        _keyRange = keyRange;
        _readPct = readPct;
        _impl = impl;
        _millis = millis;
    }

    public void run() {
        try {
            runImpl();
        } catch (final InterruptedException xx) {
            throw new Error("unexpected", xx);
        } catch (final BrokenBarrierException xx) {
            throw new RuntimeException("unexpected", xx);
        }
    }

    private void runImpl() throws InterruptedException, BrokenBarrierException {
        System.out.printf("%2d threads, %16s, range %d, %d%% read: ",
                _numThreads, _impl.getClass().getSimpleName(), _keyRange, _readPct);

        final CyclicBarrier barrier = new CyclicBarrier(_numThreads + 1);

        final Thread[] threads = new Thread[_numThreads];
        final long[] results = new long[_numThreads];

        for (int i = 0; i < threads.length; ++i) {
            final int index = i;
            threads[i] = new Thread("worker #" + i) {
                public void run() {
                    final Random rand = new Random(index);
                    try {
                        barrier.await();
                    } catch (final InterruptedException xx) {
                        throw new Error("unexpected", xx);
                    } catch (final BrokenBarrierException xx) {
                        throw new RuntimeException("unexpected", xx);
                    }
                    int count = 0;
                    while (!_done) {
                        final int key = rand.nextInt(_keyRange);
                        final int pct = rand.nextInt(200);
                        if (pct < _readPct * 2) {
                            _impl.contains(key);
                        } else if ((pct & 1) == 0) {
                            _impl.add(key);
                        } else {
                            _impl.remove(key);
                        }
                        ++count;
                    }
                    results[index] = count;
                }
            };
        }

        for (Thread t : threads) {
            t.start();
        }
        barrier.await();
        Thread.sleep(_millis);
        _done = true;

        for (Thread t : threads) {
            t.join();
        }

        long total = 0;
        for (long c : results) {
            total += c;
        }

        System.out.printf("%9d operations/sec\n", total * 1000L / _millis);
    }
}