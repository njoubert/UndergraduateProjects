package nachos.threads;

import nachos.ag.BoatGrader;
import nachos.machine.Lib;

public class Boat
{
    static BoatGrader bg;
    
    enum Location {
        OAHU, MOLOKAI;
    }
    
    // Make informational variables
    static Location boatLocation;
    static int numAdultsOnMolokai, numAdultsOnOahu, numChildrenOnMolokai, numChildrenOnOahu;
    static boolean boatHasPilot;
    static boolean boatHasPassenger;
    static int numChildrenDone;
    
    // Make locks
    static Lock lock;
    static Lock finishLock;
    
    // Make conditional variables
    static Condition passengerWanted;
    static Condition passengerReady;
    static Condition adultOnOahu;
    static Condition childOnOahu;
    static Condition childOnMolokai;
    static Condition finished;
    
    public static void selfTest()
    {
	BoatGrader b = new BoatGrader();
	
//	System.out.println("\n ***Testing Boats with only 2 children***");
//	begin(0, 2, b);

//	System.out.println("\n ***Testing Boats with 2 children, 1 adult***");
//  	begin(1, 2, b);

//  	System.out.println("\n ***Testing Boats with 3 children, 3 adults***");
//  	begin(3, 3, b);
  	
  	System.out.println("\n ***Testing Boats with 6 children, 6 adults***");
        begin(100, 125, b);
    }

    public static void begin( int adults, int children, BoatGrader b )
    {
	// Store the externally generated autograder in a class
	// variable to be accessible by children.
	bg = b;

	// Instantiate global variables here
	lock = new Lock();
	finishLock = new Lock();
	boatLocation = Location.OAHU;
	passengerWanted = new Condition(lock);
	passengerReady = new Condition(lock);
	adultOnOahu = new Condition(lock);
	childOnOahu = new Condition(lock);
	childOnMolokai = new Condition(lock);
	finished = new Condition(finishLock);
	numAdultsOnMolokai = numAdultsOnOahu = numChildrenOnMolokai = numChildrenOnOahu = 0;
	boatHasPilot = false;
	boatHasPassenger = false;
	
	// Create threads here. See section 3.4 of the Nachos for Java
	// Walkthrough linked from the projects page.
	KThread t;

	// Make all the KThreads containing Hawaiian children
        for (int i = 0; i < children; i++) {
            Runnable child = new Runnable() {
                public void run() {
                    ChildItinerary();
                }
            };
            t = new KThread(child);
            t.setName("Child Thread #" + i);
            t.fork();
        }
        
	// Make all the KThreads containing Hawaiian adults
	for (int i = 0; i < adults; i++) {
	    Runnable adult = new Runnable() {
	        public void run() {
	            AdultItinerary();
	        }
	    };
            t = new KThread(adult);
            t.setName("Adult Thread #" + i);
            t.fork();
        }
	
	finishLock.acquire();
	do {
	    finished.sleep();
	} while (numAdultsOnMolokai != adults && numChildrenOnMolokai != children);
	finishLock.release();
	
	
    }

    static void AdultItinerary()
    {
	/* This is where you should put your solutions. Make calls
	   to the BoatGrader to show that it is synchronized. For
	   example:
	       bg.AdultRowToMolokai();
	   indicates that an adult has rowed the boat across to Molokai
	*/
        lock.acquire();
        
        numAdultsOnOahu++;
        adultOnOahu.sleep();
        Lib.assertTrue(numChildrenOnOahu == 1);
        adultRowsToMolokai();
        Lib.assertTrue(numChildrenOnMolokai>0);
        childOnMolokai.wake();
        
        lock.release();
    }

    static void ChildItinerary()
    {
        lock.acquire();
        
        boolean done = false;
        Location myLocation = Location.OAHU;
        numChildrenOnOahu++;
        
        if (numChildrenOnOahu == 2) {
            childOnOahu.wake();
        }
        childOnOahu.sleep();

        while (!done) {
            // Kinda hacky.  This whole thing shouldn't be run in the first place
            // Safety net in case some things make it through
            // In Willy's mind, you shouldn't have people getting here, but you might
            while (boatLocation != myLocation) {
                if (myLocation == Location.OAHU) {
                    TestStubs.debugPrint("Silly child harmlessly falls asleep on Oahu");
                    childOnOahu.sleep();
                } else {
                    TestStubs.debugPrint("*** THIS SHOULD NEVER BE RUN ***");
                    childOnMolokai.sleep();
                }
            }
            
            if (myLocation == Location.OAHU) {
                if (numChildrenOnOahu >= 2) {
                    if (!boatHasPilot) {
                        boatHasPilot = true;
                        childOnOahu.wake();
                        passengerWanted.sleep();
                        Lib.assertTrue(boatHasPassenger);
                        passengerReady.wake();
                        
                        childRowsToMolokai();
                        myLocation = Location.MOLOKAI;
                        boatHasPilot = false;
                        childOnMolokai.sleep();
                    }
                    else if (!boatHasPassenger) {
                        Lib.assertTrue(boatHasPilot);
                        boatHasPassenger = true;
                        passengerWanted.wake();
                        passengerReady.sleep();
                        
                        childRidesToMolokai();
                        myLocation = Location.MOLOKAI;
                        boatHasPassenger = false;
                        childOnMolokai.wake();
                        if ((numChildrenOnMolokai-numChildrenDone) >= 3) {
                            done = true;
                            numChildrenDone++;
                        }
                        else {
                            childOnMolokai.sleep();
                        }
                    }
                    else {
                        TestStubs.debugPrint("*** You bastard child.  Why are you here?? *WHACK* *WHACK* ***");
                        childOnOahu.sleep();
                    }
                }
                else {
                    childRowsToMolokai();
                    myLocation = Location.MOLOKAI;
                    // We're done!!
                    finishLock.acquire();
                    finished.wake();
                    finishLock.release();
                    ThreadedKernel.alarm.waitUntil(1000);
                }
            }
            else {
                childRowsToOahu();
                myLocation = Location.OAHU;
                
                if (numChildrenOnOahu > 1) {
                    // You'll wake up another passenger when you become pilot,
                    // so don't worry about waking up a passenger right now
                } else if (numAdultsOnOahu > 0) {
                    adultOnOahu.wake();
                    childOnOahu.sleep();
                }
                
            }
        }
        
        lock.release();
    }

    static void adultRowsToMolokai() {
        numAdultsOnOahu--;
        numAdultsOnMolokai++;
        boatLocation = Location.MOLOKAI;
        bg.AdultRowToMolokai();
    }
    
    static void adultRowsToOahu() {
        numAdultsOnMolokai--;
        numAdultsOnOahu++;
        boatLocation = Location.OAHU;
        bg.AdultRowToOahu();
    }

    static void childRowsToMolokai() {
        numChildrenOnOahu--;
        numChildrenOnMolokai++;
        boatLocation = Location.MOLOKAI;
        bg.ChildRowToMolokai();
    }

    static void childRidesToMolokai() {
        numChildrenOnOahu--;
        numChildrenOnMolokai++;
        bg.ChildRideToMolokai();
    }

    static void childRowsToOahu() {
        numChildrenOnMolokai--;
        numChildrenOnOahu++;
        boatLocation = Location.OAHU;
        bg.ChildRowToOahu();
    }
    
    static void SampleItinerary()
    {
	// Please note that this isn't a valid solution (you can't fit
	// all of them on the boat). Please also note that you may not
	// have a single thread calculate a solution and then just play
	// it back at the autograder -- you will be caught.
	System.out.println("\n ***Everyone piles on the boat and goes to Molokai***");
	bg.AdultRowToMolokai();
	bg.ChildRideToMolokai();
	bg.AdultRideToMolokai();
	bg.ChildRideToMolokai();
    }
    
}
