/********* Miscellaneous routines begin here                         *********/
/**                                                                         **/
/**                                                                         **/

/* Random number seed is not constant, but I've made it global anyway.       */
/*   This should not hurt reentrancy (unless you want repeatability).        */

unsigned long randomseed = 1;                 /* Current random number seed. */

/*****************************************************************************/
/*                                                                           */
/*  randomnation()   Generate a random number between 0 and `choices' - 1.   */
/*                                                                           */
/*  This is a simple linear congruential random number generator.  Hence, it */
/*  is a bad random number generator, but good enough for most randomized    */
/*  geometric algorithms.                                                    */
/*                                                                           */
/*****************************************************************************/

unsigned long randomnation(unsigned long choices)
{
  unsigned long newrandom;

  if (choices >= 714025lu) {
    newrandom = (randomseed * 1366lu + 150889lu) % 714025lu;
    randomseed = (newrandom * 1366lu + 150889lu) % 714025lu;
    newrandom = newrandom * (choices / 714025lu) + randomseed;
    if (newrandom >= choices) {
      return newrandom - choices;
    } else {
      return newrandom;
    }
  } else {
    randomseed = (randomseed * 1366lu + 150889lu) % 714025lu;
    return randomseed % choices;
  }
}

/**                                                                         **/
/**                                                                         **/
/********* Miscellaneous routines end here                           *********/


