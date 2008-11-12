package tracker;

import java.util.Stack;

/** Methods for computing collision times and effects on round equal-sized
 *  moving objects in 2D.  Don't change this class.
 *
 *  <p> For the purposes of this package, we represent the state of an
 *  object as a four-element double[] containing [x, y, vx, vy]: the
 *  coordinates of the object's center, and the x- and y-components of 
 *  its velocity.   
 * 
 *  <p> The vector-manipulating routines add, len2, len, and ip operate
 *  on 2-element segments of arrays of doubles.  To specify such a 
 *  segment, you supply an array and the index of the first element of 
 *  the segment.  Thus, if A, V, and C are 4-element arrays, then 
 *  add (C, 0, A, 0, t, V, 2) adds (A[0], A[1]) + t * (V[2], V[3]) and
 *  places the result in C[0], C[1].  In comments, we use the notation
 *  X[i:i+1] to refer to one of these two-element segments.
 *
 *  <p> The computations are not exactly physical (especially when the
 *  objects interpenetrate slightly due to round-off), but they should do
 *  for this project. */
public class Physics {

  /** Given two objects with radius R with states P0 and P1 (see the 
   *  class documentation for ucb.proj2.Physics, above), compute
   *  the time at which they next collide, assuming that initially, 
   *  they are separated by a distance of >= 2R.  Objects that will not
   *  collide return a result of positive infinity
   *  (Double.POSITIVE_INFINITY). Objects whose centers are moving away
   *  from each others are taken not to collide. */  
  public static double collide (double[] p0, double[] p1, double r) {
    
    double[] 
      dx = add (p1, 0, -1.0, p0, 0),
      dv = add (p1, 2, -1.0, p0, 2);
    double dx2 = len2 (dx,0), dv2 = len2 (dv,0), xv = ip (dx, 0, dv, 0);

    double disc2 = xv * xv - dv2 * (dx2 - 4 * sqr (r));

    recycle (dx); recycle (dv);
    if (disc2 < 0.0 || dv2 == 0.0 || xv >= 0)
      return Double.POSITIVE_INFINITY;

    return Math.max (-xv - Math.sqrt (disc2), 0.0) / dv2;
  }

  /** Given two objects with P0 and P1 (see the class documentation for 
   *  ucb.proj2.Physics, above) that are in collision with each other,
   *  (destructively) modify their velocities as a result of that 
   *  collision.  The objects are assumed to be spherical and of
   *  equal radius, the collision is assumed to be elastic (kinetic-
   *  energy preserving), and the material is assumed to be extremely
   *  stiff and slippery.  */
  public static void rebound (double[] p0, double[] p1) {
    double[] 
      dx = add (p1, 0, -1.0, p0, 0),
      dv = add (p1, 2, -1.0, p0, 2);
    double dx2 = len2 (dx, 0), xv = ip (dx, 0, dv, 0), c = -xv / dx2;

    if (c > 0.0) {
      add (p1, 2, p1, 2, c, dx, 0);
      add (p0, 2, p0, 2, -c, dx, 0);
    }
    recycle (dx); recycle (dv);
  }

  /* Vector operations */

  /** Set Z[IZ : IZ+1] to AX*X[IX : IX+1] + AY*Y[IY : IY+1], returning
   *  Z. */
  public static double[] add (double[] Z, int iz, 
			      double ax, double[] X, int ix, 
			      double ay, double[] Y, int iy) {
    Z[iz] = ax*X[ix] + ay*Y[iy];
    Z[iz+1] = ax*X[ix+1] + ay*Y[iy+1];
    return Z;
  }

  /** Same as add (Z, IZ, 1.0, X, IX, 1.0, Y, IY). */
  public static double[] add (double[] Z, int iz, double[] X, int ix, 
			      double[] Y, int iy) {
    return add (Z, iz, 1.0, X, ix, 1.0, Y, iy);
  }

  /** Same as add (Z, IZ, 1.0, X, IX, AY, Y, IY). */
  public static double[] add (double[] Z, int iz, double[] X, int ix, 
			      double ay, double[] Y, int iy) {
    return add (Z, iz, 1.0, X, ix, ay, Y, iy);
  }
  
  /** Same as add (NZ, 0, AX, X, IX, AY, Y, IY), where NZ is a new
   *  double[2].  */
  public static double[] add (double ax, double[] X, int ix, 
			      double ay, double[] Y, int iy) {
    return add (alloc (), 0, ax, X, ix, ay, Y, iy);
  }

  /** Same as add (NZ, 0, 1.0, X, IX, AY, Y, IY), where NZ is a new
   *  double[2].  */
  public static double[] add (double[] X, int ix, 
			      double ay, double[] Y, int iy) {
    return add (alloc (), 0, 1.0, X, ix, ay, Y, iy);
  }

  /** The inner product X[IX] * Y[IY] + X[IX+1] * Y[IY+1]. */
  public static double ip (double[] X, int ix, double[] Y, int iy) {
    return X[ix] * Y[iy] + X[ix+1] * Y[iy+1];
  }

  /** The squared length of X[IX : IX+1]. */
  public static double len2 (double[] X, int ix) {
    return sqr (X[ix]) + sqr (X[ix+1]);
  }

  /** The length of X[IX : IX+1] */
  public static double len (double[] X, int ix) {
    return Math.sqrt (len2 (X, ix));
  }

  /** The squared length of X[IX : IX+1] - Y[IY : IY+1]. */
  public static double len2 (double[] X, int ix, double Y[], int iy) {
    return sqr (X[ix]-Y[iy]) + sqr (X[ix+1]-Y[iy+1]);
  }

  /** The length of X[IX : IX+1] - Y[IY : IY+1]. */
  public static double len (double[] X, int ix, double Y[], int iy) {
    return Math.sqrt (len2 (X, ix, Y, iy));
  }

  /** X ** 2 */
  private static double sqr (double x) {
    return x * x;
  }

  /** A resevoir of recyclable temporary storage vectors. */
  private static Stack<double[]> space = new Stack<double[]> ();

  /** Allocate and return a 2-element vector (new or recycled, 
   *  as available). */
  private static double[] alloc () {
    if (space.isEmpty ())
      return new double[2];
    else 
      return space.pop ();
  }

  /** Treat X as if it will no longer be used unless later returned 
   *  by alloc (). */
  private static void recycle (double[] x) {
    space.push (x);
  }

}
