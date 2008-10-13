/********* Point location routines begin here                        *********/
/**                                                                         **/
/**                                                                         **/

#define ONETHIRD 0.333333333333333333333333333333333333333333333333333333333333

/*  Labels that signify the result of point location.  The result of a       */
/*  search indicates that the point falls in the interior of a tetrahedron,  */
/*  on a face, on an edge, on a vertex, or outside the mesh.                 */

enum locateresult {INTETRA, ONFACE, ONEDGE, ONVERTEX, OUTSIDE, BADSTART};

/*****************************************************************************/
/*                                                                           */
/*  locatepoint()   Find a tetrahedron containing a given point--or, if the  */
/*                  point is outside the triangulation, a triangular face    */
/*                  that the point is beyond.                                */
/*                                                                           */
/*  This procedure requires the caller to specify the identity of a          */
/*  triangular face of the triangulation (which cannot be a ghost triangle). */
/*  The procedure uses walking point location, starting from that face.      */
/*                                                                           */
/*  The output-only array `foundtet' contains the tetrahedron on return,     */
/*  with positive orientation.  If `vertexlocate' is outside the             */
/*  triangulation, then foundtet[3] is GHOSTVERTEX, and the first three tags */
/*  are a triangular face that the `vertexlocate' is beyond.                 */
/*                                                                           */
/*  WARNING:  This routine is designed for convex tetrahedralizations, and   */
/*  will not generally work if there are holes or concavities.  However, it  */
/*  is useful in Delaunay refinement algorithms for finding the circumcenter */
/*  of a tetrahedron, so long as the search begins from a face of the        */
/*  tetrahedron in question.                                                 */
/*                                                                           */
/*  This procedure does not require that the triangulation have ghost        */
/*  tetrahedra (and doesn't use them if it does).                            */
/*                                                                           */
/*  Returns ONVERTEX if the point lies on an existing vertex.  `foundtet'    */
/*  is ordered so foundtet[0] coincides with the point.                      */
/*                                                                           */
/*  Returns ONEDGE if the point lies on a mesh edge.  `foundtet' is ordered  */
/*  so foundtet[0] and foundtet[1] are the endpoints of that edge.           */
/*                                                                           */
/*  Returns ONFACE if the point lies on a mesh face.  `foundtet' is ordered  */
/*  so foundtet[0] through foundtet[2] are the vertices of that face.        */
/*                                                                           */
/*  Returns INTETRA if the point lies strictly inside a tetrahedron.         */
/*  `foundtet' holds the vertices of that tetrahedron.                       */
/*                                                                           */
/*  Returns OUTSIDE if the point lies outside the mesh.  foundtet[0] through */
/*  foundtet[2] are the vertices of a triangular face that `vertexlocate' is */
/*  beyond.  foundtet[3] is GHOSTVERTEX.  The first three vertices are       */
/*  oriented so that if foundtet[3] were replaced by `vertexlocate', the     */
/*  tetrahedron would have positive orientation.  (This means that           */
/*  foundtet[0] through foundtet[2] are in clockwise order as viewed by      */
/*  `vertexlocate'.)                                                         */
/*                                                                           */
/*  Returns BADSTART if the specified face is not in the triangulation.      */
/*  In that case, `foundtet' is left untouched.                              */
/*                                                                           */
/*****************************************************************************/

enum locateresult locatepoint(struct behavior *behave,
                              struct tetcomplex *plex,
                              tag vertexlocate,
                              tag vertex1,
                              tag vertex2,
                              tag vertex3,
                              tag foundtet[4])
{
  tag adjacencies[2];
  starreal centroid[3];
  starreal *pointlocate, *point1, *point2, *point3, *pointapex;
  starreal *swappoint;
  starreal orient1, orient2, orient3, startorient;
  tag apex;
  tag swaptag;

  pointlocate = ((struct vertex *) tetcomplextag2vertex(plex,
                                                        vertexlocate))->coord;
  if (behave->verbose > 2) {
    printf("      Searching for vertex # %lu (tag %lu):\n",
           (unsigned long)
           ((struct vertex *) tetcomplextag2vertex(plex, vertexlocate))
           ->number,
           (unsigned long) vertexlocate);
    printf("        (%.12g, %.12g, %.12g).\n", (double) pointlocate[0],
           (double) pointlocate[1], (double) pointlocate[2]);
  }

  if (!tetcomplexadjacencies(plex, vertex1, vertex2, vertex3,
                             adjacencies)) {
    return BADSTART;
  }
  if (adjacencies[1] == GHOSTVERTEX) {
    adjacencies[1] = adjacencies[0];
    adjacencies[0] = GHOSTVERTEX;
    swaptag = vertex2;
    vertex2 = vertex3;
    vertex3 = swaptag;
  }
  apex = adjacencies[1];

  /* Where are we? */
  point1 = ((struct vertex *) tetcomplextag2vertex(plex, vertex1))->coord;
  point2 = ((struct vertex *) tetcomplextag2vertex(plex, vertex2))->coord;
  point3 = ((struct vertex *) tetcomplextag2vertex(plex, vertex3))->coord;

  /* Is `pointlocate' a vertex of the starting triangle? */
  if ((point1[0] == pointlocate[0]) && (point1[1] == pointlocate[1]) &&
      (point1[2] == pointlocate[2])) {
    foundtet[0] = vertex1;
    foundtet[1] = vertex2;
    foundtet[2] = vertex3;
    foundtet[3] = apex;
    return ONVERTEX;
  }
  if ((point2[0] == pointlocate[0]) && (point2[1] == pointlocate[1]) &&
      (point2[2] == pointlocate[2])) {
    foundtet[0] = vertex2;
    foundtet[1] = vertex3;
    foundtet[2] = vertex1;
    foundtet[3] = apex;
    return ONVERTEX;
  }
  if ((point3[0] == pointlocate[0]) && (point3[1] == pointlocate[1]) &&
      (point3[2] == pointlocate[2])) {
    foundtet[0] = vertex3;
    foundtet[1] = vertex1;
    foundtet[2] = vertex2;
    foundtet[3] = apex;
    return ONVERTEX;
  }

  startorient = orient3d(behave, point1, point2, point3, pointlocate);
  if (startorient < 0.0) {
    swaptag = vertex2;
    vertex2 = vertex3;
    vertex3 = swaptag;
    apex = adjacencies[0];
    swappoint = point2;
    point2 = point3;
    point3 = swappoint;
  }

  /* We will walk along a straight line from the centroid of the starting */
  /*   face to the point we are searching for.                            */
  centroid[0] = ONETHIRD * (point1[0] + point2[0] + point3[0]);
  centroid[1] = ONETHIRD * (point1[1] + point2[1] + point3[1]);
  centroid[2] = ONETHIRD * (point1[2] + point2[2] + point3[2]);

  while (1) {
    /* Check for walking out of the tetrahedralization. */
    if (apex == GHOSTVERTEX) {
      foundtet[0] = vertex1;
      foundtet[1] = vertex2;
      foundtet[2] = vertex3;
      foundtet[3] = apex;
      return OUTSIDE;
    }
    pointapex = ((struct vertex *) tetcomplextag2vertex(plex, apex))->coord;

    if (plex->verbosity > 3) {
      printf("        At tag %lu (%.12g, %.12g, %.12g)"
             " tag %lu (%.12g, %.12g, %.12g)\n",
             (unsigned long) vertex1, (double) point1[0],
             (double) point1[1], (double) point1[2],
             (unsigned long) vertex2, (double) point2[0],
             (double) point2[1], (double) point2[2]);
      printf("           tag %lu (%.12g, %.12g, %.12g)"
             " tag %lu (%.12g, %.12g, %.12g)\n",
             (unsigned long) vertex3, (double) point3[0],
             (double) point3[1], (double) point3[2],
             (unsigned long) apex, (double) pointapex[0],
             (double) pointapex[1], (double) pointapex[2]);
    }

    /* Check whether the apex is the point we seek. */
    if ((pointapex[0] == pointlocate[0]) && (pointapex[1] == pointlocate[1]) &&
        (pointapex[2] == pointlocate[2])) {
      foundtet[0] = apex;
      foundtet[1] = vertex3;
      foundtet[2] = vertex2;
      foundtet[3] = vertex1;
      return ONVERTEX;
    }

    /* Find the orientation of `pointlocate' relative to three of the faces */
    /*   of the tetrahedron (leaving out the face where we started.)        */
    orient1 = orient3d(behave, point2, point3, pointapex, pointlocate);
    orient2 = orient3d(behave, point3, point1, pointapex, pointlocate);
    orient3 = orient3d(behave, point1, point2, pointapex, pointlocate);

    if (orient3 > 0.0) {
      if (orient2 > 0.0) {
        if (orient1 > 0.0) {
          /* Any of the three faces is a viable next step.  Discriminate */
          /*   by using the line from the centroid to the point sought.  */
          if (orient3d(behave,
                       centroid, point1, pointapex, pointlocate) > 0.0) {
            if (orient3d(behave,
                         centroid, point3, pointapex, pointlocate) > 0.0) {
              /* Move through the face opposite vertex1. */
              vertex1 = apex;
              point1 = pointapex;
            } else {
              /* Move through the face opposite vertex2. */
              vertex2 = apex;
              point2 = pointapex;
            }
          } else if (orient3d(behave, centroid, point2, pointapex,
                              pointlocate) > 0.0) {
            /* Move through the face opposite vertex3. */
            vertex3 = apex;
            point3 = pointapex;
          } else {
            /* Move through the face opposite vertex1. */
            vertex1 = apex;
            point1 = pointapex;
          }
        } else {
          /* Two faces, opposite vertex2 and vertex3, are viable. */
          if (orient3d(behave,
                       centroid, point1, pointapex, pointlocate) > 0.0) {
            vertex2 = apex;
            point2 = pointapex;
          } else {
            vertex3 = apex;
            point3 = pointapex;
          }
        }
      } else if (orient1 > 0.0) {
        /* Two faces, opposite vertex1 and vertex3, are viable. */
        if (orient3d(behave, centroid, point2, pointapex, pointlocate) > 0.0) {
          vertex3 = apex;
          point3 = pointapex;
        } else {
          vertex1 = apex;
          point1 = pointapex;
        }
      } else {
        /* Only one face, opposite vertex3, is viable. */
        vertex3 = apex;
        point3 = pointapex;
      }
    } else if (orient2 > 0.0) {
      if (orient1 > 0.0) {
        /* Two faces, opposite vertex1 and vertex2, are viable. */
        if (orient3d(behave, centroid, point3, pointapex, pointlocate) > 0.0) {
          vertex1 = apex;
          point1 = pointapex;
        } else {
          vertex2 = apex;
          point2 = pointapex;
        }
      } else {
        /* Only one face, opposite vertex2, is viable. */
        vertex2 = apex;
        point2 = pointapex;
      }
    } else if (orient1 > 0.0) {
      /* Only one face, opposite vertex1, is viable. */
      vertex1 = apex;
      point1 = pointapex;
    } else {
      /* The point we seek must be on the boundary of or inside this */
      /*   tetrahedron.  Check for boundary cases.                   */
      if (startorient == 0.0) {
        foundtet[3] = apex;
        if (orient1 == 0.0) {
          foundtet[0] = vertex2;
          foundtet[1] = vertex3;
          foundtet[2] = vertex1;
          return ONEDGE;
        }
        if (orient2 == 0.0) {
          foundtet[0] = vertex3;
          foundtet[1] = vertex1;
          foundtet[2] = vertex2;
          return ONEDGE;
        }
        foundtet[0] = vertex1;
        foundtet[1] = vertex2;
        foundtet[2] = vertex3;
        if (orient3 == 0.0) {
          return ONEDGE;
        }
        /* `pointlocate' is in the relative interior of this face. */
        return ONFACE;
      }
      if (orient3 == 0.0) {
        foundtet[3] = vertex3;
        if (orient2 == 0.0) {
          foundtet[0] = vertex1;
          foundtet[1] = apex;
          foundtet[2] = vertex2;
          return ONEDGE;
        }
        foundtet[0] = apex;
        foundtet[1] = vertex2;
        foundtet[2] = vertex1;
        if (orient1 == 0.0) {
          return ONEDGE;
        }
        return ONFACE;
      }
      if (orient2 == 0.0) {
        foundtet[0] = vertex3;
        foundtet[1] = apex;
        foundtet[2] = vertex1;
        foundtet[3] = vertex2;
        if (orient1 == 0.0) {
          return ONEDGE;
        }
        return ONFACE;
      }
      foundtet[0] = apex;
      foundtet[1] = vertex3;
      foundtet[2] = vertex2;
      foundtet[3] = vertex1;
      if (orient1 == 0.0) {
        return ONFACE;
      }
      return INTETRA;
    }

    /* Move to another tetrahedron. */
    if (!tetcomplexadjacencies(plex, vertex1, vertex2, vertex3, adjacencies)) {
      printf("Internal error in locatepoint():\n");
      printf("  Triangular face is present yet not present.\n");
      internalerror();
    }
    apex = adjacencies[1];
    startorient = 1.0;
  }
}

/**                                                                         **/
/**                                                                         **/
/********* Point location routines end here                          *********/


