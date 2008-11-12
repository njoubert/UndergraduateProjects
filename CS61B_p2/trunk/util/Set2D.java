/* Do NOT modify the contents of this file.  It defines an API that we
 * will assume exists. */

package util;

/** A set of moveable objects in a rectangular region of 2D space, 
 *  retrievable by position.  Each object has a unique non-negative 
 *  integer identifier.  Implementations may assume that the maximum 
 *  identifier is a small multiple of SIZE ().  All objects in a Set2D 
 *  reside within a bounding rectangle, whose coordinates are 
 *  retrievable. */
public interface Set2D {

    /** The current number of objects in the set. */
  int size ();

  /** Add a point unique identifier ID and initial position (X,Y) 
   *  to THIS.  Causes an IllegalArgumentException if ID < 0, (X,Y) is not 
   *  in the bounding rectangle, or if exists (ID).  */
  void add (int id, double x, double y);

  /** Modify the point with unique identifier ID in THIS to have 
   *  position (X,Y) in THIS.  Causes an IllegalArgumentException 
   *  if ! exists (ID) or if (X,Y) is not within the bounding rectangle. */
  void set (int id, double x, double y);

  /** Remove the object whose unique identifier is ID, if any, and 
   *  otherwise do nothing. */
  void remove (int id);

  /** True iff ID is the unique id of an object in THIS. */
  boolean exists (int id);

  /** The smallest x-coordinate allowed for objects in THIS.  All
   *  objects must have an x-coordinate that is >= llx (). */
  double llx ();

  /** The smallest y-coordinate allowed for objects in THIS.  All
   *  objects must have an y-coordinate that is >= lly (). */
  double lly ();

  /** The largest x-coordinate allowed for objects in THIS.  All
   *  objects must have an x-coordinate that is < urx (). */
  double urx ();

  /** The largest y-coordinate allowed for objects in THIS.  All
   *  objects must have an y-coordinate that is < ury (). */
  double ury ();

  /* For any unique identifier that is not that of an object currently in
   * the set, the following routines return Double.NaN (Not a Number), 
   * a special value of type double that you may test for with 
   * Double.isNaN (v).  */

  /** The x-coordinate of the object whose unique identifier is ID, if
   *  any; otherwise NaN.  */
  double x (int id);

  /** The y-coordinate of the object whose unique identifier is ID, if
   *  any; otherwise NaN.  */
  double y (int id);

  /** A Set2DIterator that returns all objects in THIS, sorted by id. */
  Set2DIterator iterator ();

  /** A Set2DIterator that returns all objects in THIS whose coordinates
   *  lie on or within a rectangle whose lower-left coordinate is 
   *  (XL, YL) and whose upper-right coordinate is (XU, YU). */
  Set2DIterator iterator (double xl, double yl, double xu, double yu);

}
  
