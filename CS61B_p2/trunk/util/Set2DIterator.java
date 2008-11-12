/* Do NOT modify the contents of this file.  It defines an API that we
 * will assume exists. */

package util;

/** An iterator over points of a Set2D.  Differs slightly from an 
 *  ordinary Java iterator in that next returns the integer id of an
 *  object, rather than some kind of Object. */

public interface Set2DIterator {

  /** True iff THIS has another object id. */
  boolean hasNext ();

  /** The id of the next object in the iteration.  Assumes hasNext (). */
  int next ();

}
