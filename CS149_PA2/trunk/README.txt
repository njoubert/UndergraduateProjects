PA2 - STM Treap

Niels Joubert - njoubert
Suhaas Prasad - suhaasp
-------------------------------------------


Transactional Memory works on the principle of optimistic concurrency.
If no memory a transaction depends on changes while a transaction is executing,
the transaction commits successfully. Otherwise a conflict is registered, and
causes the transaction to completely retry.

The current implementation of the Treap assignes pointer values as it
recursively traverses down the tree. This assignment means that DueceSTM
will abort any transactions that tries to run concurrently, even if the
transaction simply traverses down the tree without actually changing anything.

We chose to delay any assignment until it is absolutely necessary. Necessity is
indicated by values changing as a tree is traversed, thus we replaced the 
assignment statements found in CoarseLockTreap with check-and-assign
statements that would not modify the pointer values of a parent node
if the child node is still the same object. This prevents writes from
cascading up the tree and interfering with reads that occur higher in the tree.

As for contention around pseudo-random numbers, we replaced the implicitly
atomic code of the randPriority() function with a tight busy loop, using
the atomic compareAndSet method of the AtomicLong class, that replaces
the long randState. This means that multiple writing threads will receive different
pseudorandom numbers and update the state of the random number generator
without causing conflicts if they update different parts of the tree. It still
generates at most one random value for each state of the generator.

Our observed performance values were better than the CoarseLockTreap across the
board, with up to 100% improvement in the case of 0% reads, when run with
DeuceSTM's TL2 algorithm on our local 8-core Xeon Hyperthreading-enabled machine.
We observed similar results on a hotbox node with 2.3Ghz and 8 cores.

