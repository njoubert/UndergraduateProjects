/********* Z-order computation routines begin here                   *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  zorderbefore()   Returns 1 if the point (x1, y1, z1) occurs before       */
/*                   (x2, y2, z2) in a z-order space-filling curve;          */
/*                   0 otherwise.  Coordinates are floating point.           */
/*                                                                           */
/*  I won't try to explain what a z-order curve is here; look it up on the   */
/*  Web.  This is a somewhat nonstandard z-order curve.  At the top level,   */
/*  points in different orthants are ordered as follows:                     */
/*                                                                           */
/*  nonnegative x, nonnegative y, negative z                                 */
/*  negative x, nonnegative y, negative z                                    */
/*  negative x, negative y, negative z                                       */
/*  nonnegative x, negative y, negative z                                    */
/*  nonnegative x, negative y, nonnegative z                                 */
/*  negative x, negative y, nonnegative z                                    */
/*  negative x, nonnegative y, nonnegative z                                 */
/*  nonnegative x, nonnegative y, nonnegative z (the positive orthant)       */
/*                                                                           */
/*  This is a one-level Hilbert curve.  Note that a zero coordinate is       */
/*  treated as a positive coordinate.  For example, (-1, 1, 1) precedes      */
/*  (0, 0, 1) in the ordering, because the latter point is treated as being  */
/*  in the positive orthant.                                                 */
/*                                                                           */
/*  In the positive orthant, this z-order curve progresses by decreasing z,  */
/*  then decreasing y, then increasing x.  For example, (0, 1, 1) precedes   */
/*  (1, 1, 1) precedes (0, 0, 1) precedes (1, 0, 1) precedes (0, 1, 0)       */
/*  precedes (1, 1, 0) precedes (0, 0, 0) precedes (1, 0, 0).  This ordering */
/*  is chosen to maximize the spatial locality of points in adjoining        */
/*  orthants.                                                                */
/*                                                                           */
/*  In the other orthants, the z-order curve is a reflection across one or   */
/*  more coordinate axes of the z-order curve in the positive orthant.  If   */
/*  the number of reflections is odd (i.e. for points with an odd number of  */
/*  negative coordinates), the z-order is also reversed; for example,        */
/*  (2, 3, 3) precedes (3, 3, 3), but (-3, 3, 3) precedes (-2, 3, 3).  This  */
/*  causes the z-ordering to adjoin nicely between orthants.                 */
/*                                                                           */
/*  This procedure requires IEEE 754 conformant floating-point double        */
/*  precision numbers.  It uses bit manipulation to extract exponents from   */
/*  floating-point numbers and to create numbers with specified exponents.   */
/*  It also uses the clever trick of exclusive-oring two significands to     */
/*  help figure out the first bit of disagreement between two floating-point */
/*  numbers with the same exponent.  Unfortunately, it needs to know the     */
/*  endianness of the hardware to work correctly.                            */
/*                                                                           */
/*  WARNING:  This routine does not work with denormalized numbers.          */
/*  I should fix that some time.                                             */
/*                                                                           */
/*  x1, y1, z1:  Coordinates of the first point to compare.                  */
/*  x2, y2, z2:  Coordinates of the second point to compare.                 */
/*                                                                           */
/*  Returns 1 if first point precedes the second in z-order; 0 otherwise.    */
/*                                                                           */
/*****************************************************************************/

int zorderbefore(starreal x1,
                 starreal y1,
                 starreal z1,
                 starreal x2,
                 starreal y2,
                 starreal z2)
{
  starreal xor, powerof2;
  long x1exp, x2exp, y1exp, y2exp, z1exp, z2exp;
  long xmax, ymax, zmax;
  int toggle;
  int endianness;

  /* For two points in the same orthant, toggle the result once for each */
  /*   negative coordinate in one of the points.                         */
  toggle = 0;
  /* The endianness of the hardware. */
  endianness = 1;

  /* Figure out which orthants the points are in. */
  if (z1 < 0.0) {
    if (z2 < 0.0) {
      /* Both points have negative z-coordinates; reverse the z-ordering. */
      toggle = 1;
      /* Reflect to positive z-coordinates. */
      z1 = -z1;
      z2 = -z2;
    } else {
      /* Negative z always precedes nonnegative z. */
      return 1;
    }
  } else if (z2 < 0.0) {
    /* Nonnegative z never precedes negative z. */
    return 0;
  }

  if (y1 < 0.0) {
    if (y2 < 0.0) {
      /* Both points have negative y-coordinates; reverse the z-ordering. */
      toggle = toggle ^ 1;
      /* Reflect to positive y-coordinates. */
      y1 = -y1;
      y2 = -y2;
    } else {
      /* Negative y precedes nonnegative y (unless z is negative). */
      return 1 ^ toggle;
    }
  } else if (y2 < 0.0) {
    /* Nonnegative y does not precede negative y (unless z is negative). */
    return toggle;
  }

  if (x1 < 0.0) {
    if (x2 < 0.0) {
      /* Both points have negative x-coordinates; reverse the z-ordering. */
      toggle = toggle ^ 1;
      /* Reflect to positive x-coordinates. */
      x1 = -x1;
      x2 = -x2;
    } else {
      /* Negative x precedes nonnegative x (unless toggled). */
      return 1 ^ toggle;
    }
  } else if (x2 < 0.0) {
    /* Nonnegative x does not precede negative x (unless toggled). */
    return toggle;
  }

  /* Determine the exponents of the floating-point numbers.  Note that IEEE  */
  /*   floating-point numbers do not express their exponents in two's        */
  /*   complement; rather, the exponent bits range from 1 to 2046, which     */
  /*   represent exponents from -1022 to 1023.  (Exponent bits of 0          */
  /*   represent denormalized numbers, and 2047 represents infinity or NaN.) */
  if (z1 == z2) {
    /* Check for the case where the two points are equal. */
    if ((x1 == x2) && (y1 == y2)) {
      /* A point does not precede itself. */
      return 0;
    }
    /* The z-coordinates are equal, so the other coordinates will determine */
    /*   which point comes first.  Set the z-exponents so small that they   */
    /*   cannot dominate the other exponents.                               */
    z1exp = -1l;
    z2exp = -1l;
  } else {
    /* Get the z-exponents by masking out the right bits. */
    z1exp = ((long *) &z1)[endianness] & 0x7ff00000l;
    z2exp = ((long *) &z2)[endianness] & 0x7ff00000l;
  }

  if (y1 == y2) {
    /* The y-coordinates are equal, so the other coordinates will determine */
    /*   which point comes first.  Set the y-exponents so small that they   */
    /*   cannot dominate the other exponents.                               */
    y1exp = -1l;
    y2exp = -1l;
  } else {
    /* Get the y-exponents by masking out the right bits. */
    y1exp = ((long *) &y1)[endianness] & 0x7ff00000l;
    y2exp = ((long *) &y2)[endianness] & 0x7ff00000l;
  }

  if (x1 == x2) {
    /* The x-coordinates are equal, so the other coordinates will determine */
    /*   which point comes first.  Set the x-exponents so small that they   */
    /*   cannot dominate the other exponents.                               */
    x1exp = -1l;
    x2exp = -1l;
  } else {
    /* Get the x-exponents by masking out the right bits. */
    x1exp = ((long *) &x1)[endianness] & 0x7ff00000l;
    x2exp = ((long *) &x2)[endianness] & 0x7ff00000l;
  }

  /* Compute the maximum z-, y-, and x-exponents. */
  zmax = z1exp > z2exp ? z1exp : z2exp;
  ymax = y1exp > y2exp ? y1exp : y2exp;
  xmax = x1exp > x2exp ? x1exp : x2exp;

  /* Repeat the following until one pair exponents clearly dominates. */
  /*   This loop iterates at most four times.                         */
  while (1) {
    if ((zmax >= ymax) && (zmax >= xmax)) {
      /* The largest z-exponent dominates, or at least equals, the x- and */
      /*   y-exponents.  Figure out if the z-exponents differ.            */
      if (z1exp < z2exp) {
        /* The z-exponent of the second point dominates, so (in the positive */
        /*   orthant) the second point precedes the first.                   */
        return toggle;
      } else if (z1exp > z2exp) {
        /* The z-exponent of the first point dominates, so (in the positive */
        /*   orthant) the first point precedes the second.                  */
        return 1 ^ toggle;
      } else {                                             /* z1exp == z2exp */
        /* Both points have the same z-exponent, so we need to determine  */
        /*   which bit of the two z-coordinates is the first to differ.   */
        /*   First, set `powerof2' to be 2^`z1exp', so `powerof2' has the */
        /*   same exponent as z1 and z2.                                  */
        ((long *) &powerof2)[!endianness] = 0l;
        ((long *) &powerof2)[endianness] = z1exp & 0x7ff00000l;
        /* Second, set `xor' to be a floating-point number whose exponent is */
        /*   `z1exp', and whose significand is the exclusive or of the       */
        /*   significands of z1 and z2--except the first bit of the          */
        /*   significand, which is the "hidden bit" of the IEEE format and   */
        /*   remains a 1.                                                    */
        ((long *) &xor)[!endianness] = ((long *) &z1)[!endianness] ^
                                       ((long *) &z2)[!endianness];
        ((long *) &xor)[endianness] = ((((long *) &z1)[endianness] ^
                                        ((long *) &z2)[endianness]) &
                                       ~0xfff00000l) | (z1exp & 0x7ff00000l);
        /* Third, subtract `powerof2' from `xor'.  Since they are both   */
        /*   positive and both have the same exponent, this operation is */
        /*   exact (no roundoff error), and the exponent of the result   */
        /*   will indicate the first bit where z1 and z2 disagree.       */
        xor -= powerof2;
        /* Determine the exponent of `xor'. */
        zmax = ((long *) &xor)[endianness] & 0x7ff00000l;
        /* If we were to cancel out the leading 1 bits of z1 and z2 that  */
        /*   agree, the larger of the two resulting numbers would have an */
        /*   exponent of `zmax'.  The exponent of the smaller one is      */
        /*   irrelevant, so set it really really small.                   */
        if (z1 > z2) {
          z1exp = zmax;
          z2exp = -1l;
        } else {
          z2exp = zmax;
          z1exp = -1l;
        }
        /* Now go through the loop again, because the z-exponent might */
        /*   or might not still dominate.                              */
      }
    } else if ((ymax > zmax) && (ymax >= xmax)) {
      /* The largest y-exponent dominates the z-exponents, and at least   */
      /*   equals the x-exponents.  Figure out if the y-exponents differ. */
      if (y1exp < y2exp) {
        /* The y-exponent of the second point dominates, so (in the positive */
        /*   orthant) the second point precedes the first.                   */
        return toggle;
      } else if (y1exp > y2exp) {
        /* The y-exponent of the first point dominates, so (in the positive */
        /*   orthant) the first point precedes the second.                  */
        return 1 ^ toggle;
      } else {
        /* Both points have the same y-exponent, so we need to determine  */
        /*   which bit of the two y-coordinates is the first to differ.   */
        /*   First, set `powerof2' to be 2^`y1exp', so `powerof2' has the */
        /*   same exponent as y1 and y2.                                  */
        ((long *) &powerof2)[!endianness] = 0l;
        ((long *) &powerof2)[endianness] = y1exp & 0x7ff00000l;
        /* Second, set `xor' to be a floating-point number whose exponent is */
        /*   `y1exp', and whose significand is the exclusive or of the       */
        /*   significands of y1 and y2--except the first bit of the          */
        /*   significand, which is the "hidden bit" of the IEEE format and   */
        /*   remains a 1.                                                    */
        ((long *) &xor)[!endianness] = ((long *) &y1)[!endianness] ^
                                       ((long *) &y2)[!endianness];
        ((long *) &xor)[endianness] = ((((long *) &y1)[endianness] ^
                                        ((long *) &y2)[endianness]) &
                                       ~0xfff00000l) | (y1exp & 0x7ff00000l);
        /* Third, subtract `powerof2' from `xor'.  Since they are both   */
        /*   positive and both have the same exponent, this operation is */
        /*   exact (no roundoff error), and the exponent of the result   */
        /*   will indicate the first bit where y1 and y2 disagree.       */
        xor -= powerof2;
        /* Determine the exponent of `xor'. */
        ymax = ((long *) &xor)[endianness] & 0x7ff00000l;
        /* If we were to cancel out the leading 1 bits of y1 and y2 that  */
        /*   agree, the larger of the two resulting numbers would have an */
        /*   exponent of `ymax'.  The exponent of the smaller one is      */
        /*   irrelevant, so set it really really small.                   */
        if (y1 > y2) {
          y1exp = ymax;
          y2exp = -1l;
        } else {
          y2exp = ymax;
          y1exp = -1l;
        }
        /* Now go through the loop again, because the y-exponent might */
        /*   or might not still dominate.                              */
      }
    } else {                               /* (xmax > ymax) && (xmax > zmax) */
      /* The largest x-exponent dominates the y- and z-exponents.  Figure */
      /*   out if the x-exponents differ.                                 */
      if (x1exp < x2exp) {
        /* The x-exponent of the second point dominates, so (in the positive */
        /*   orthant) the first point precedes the second.                   */
        return 1 ^ toggle;
      } else if (x1exp > x2exp) {
        /* The x-exponent of the first point dominates, so (in the positive */
        /*   orthant) the second point precedes the first.                  */
        return toggle;
      } else {
        /* Both points have the same x-exponent, so we need to determine  */
        /*   which bit of the two x-coordinates is the first to differ.   */
        /*   First, set `powerof2' to be 2^`x1exp', so `powerof2' has the */
        /*   same exponent as x1 and x2.                                  */
        ((long *) &powerof2)[!endianness] = 0l;
        ((long *) &powerof2)[endianness] = x1exp & 0x7ff00000l;
        /* Second, set `xor' to be a floating-point number whose exponent is */
        /*   `x1exp', and whose significand is the exclusive or of the       */
        /*   significands of x1 and x2--except the first bit of the          */
        /*   significand, which is the "hidden bit" of the IEEE format and   */
        /*   remains a 1.                                                    */
        ((long *) &xor)[!endianness] = ((long *) &x1)[!endianness] ^
                                       ((long *) &x2)[!endianness];
        ((long *) &xor)[endianness] = ((((long *) &x1)[endianness] ^
                                        ((long *) &x2)[endianness]) &
                                       ~0xfff00000l) | (x1exp & 0x7ff00000l);
        /* Third, subtract `powerof2' from `xor'.  Since they are both   */
        /*   positive and both have the same exponent, this operation is */
        /*   exact (no roundoff error), and the exponent of the result   */
        /*   will indicate the first bit where x1 and x2 disagree.       */
        xor -= powerof2;
        /* Determine the exponent of `xor'. */
        xmax = ((long *) &xor)[endianness] & 0x7ff00000l;
        /* If we were to cancel out the leading 1 bits of x1 and x2 that  */
        /*   agree, the larger of the two resulting numbers would have an */
        /*   exponent of `xmax'.  The exponent of the smaller one is      */
        /*   irrelevant, so set it really really small.                   */
        if (x1 > x2) {
          x1exp = xmax;
          x2exp = -1l;
        } else {
          x2exp = xmax;
          x1exp = -1l;
        }
        /* Now go through the loop again, because the x-exponent might */
        /*   or might not still dominate.                              */
      }
    }
  }
}

/**                                                                         **/
/**                                                                         **/
/********* Z-order computation routines end here                     *********/


