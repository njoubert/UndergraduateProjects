CS149 PA1
Niels Joubert

README
======================================

We are implementing a multithreaded chatroom server.
Each chatroom is represented by a ChatState.
The protocol is stateless, thus each message request (send or poll) is routed to the relevant ChatState.

The behaviour we want is:
	- a thread for each incoming request
	- thread-safe chatroom selection and creation
	- thread-safe message posting and polling for a specific chatroom
	- delaying of polling requests if no new data is available
	
Let's consider how I implemented this for correctness:
---------

ChatServer:

Each incoming request is received in runForever() by the main thread, 
where a new Socket connection object is created.
This Socket connection is individual for each incoming request since it is a stateless protocol. Inside
the while (true) loop that runs forever, we create a new local variable for the connection, and create a thread
that references this local variable. This operation is thread safe, since the local variable is only in scope 
for that specific iteration of the while loop, and the thread can safely reference it since it is a final variable.

The only other part of the ChatServer that we need to worry about is getting and creating a ChatState. 
We don't want to get into a race condition where multiple copies of a ChatState exists for the same
room, so we synchronize the getState method. This means only one thread can ever get (or create) a chatstate.

ChatState:

This is a type of producer-consumer problem, but since the consumers don't actually remove data it's much
simplified. We can serialize access to this object and allow only one thread at a time to modify the 
state of the room. This is the easiest way to ensure no race conditions or deadlock. A more complicated
version will allow multiple recentMessages to run concurrently, but only one addMessage and only if no
recentMessages are in progress.

To have the polling requests correctly delayed, we use the wait queue of the Lock object this object exposes.
If no messages are available for recentMessages to consume, we wait(); When the addMessage completes, we
notifyAll(); so that we don't have any lost notifies. wait() and notifyAll() correctly acquires and 
relinquishes the Lock.

 