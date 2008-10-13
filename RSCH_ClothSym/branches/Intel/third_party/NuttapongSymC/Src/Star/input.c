/********* Input routines begin here                                 *********/
/**                                                                         **/
/**                                                                         **/

/*  A vertex is assigned the `number' DEADVERTEX when it is deallocated, to  */
/*  indicate that the vertex record should not be consulted (and its memory  */
/*  might be reclaimed by another vertex any time).  A vertex is most likely */
/*  to become dead if it is a non-corner node of an input mesh (such as the  */
/*  side-edge nodes of a quadratic element), or if it was generated (not an  */
/*  input vertex) then subsequently deleted from a mesh.                     */

#define DEADVERTEX ((arraypoolulong) ~0)
#define ACTIVEVERTEX ((arraypoolulong) ~1)

typedef int vertexmarktype;

struct vertex {
  starreal coord[3];
  vertexmarktype mark;
  arraypoolulong number;
};

struct vertexshort {
  vertexmarktype mark;
  starreal coord[3];
};

struct segment {
  tag endpoint[2];
  struct segment *nextsegment[2];
  vertexmarktype mark;
  short acute;
  arraypoolulong number;
};

struct facet {
  vertexmarktype mark;
  arraypoolulong number;
};

struct inputs {
  tag *vertextags;
  arraypoolulong vertexcount;                   /* Number of input vertices. */
  unsigned int attribcount;              /* Number of attributes per vertex. */
  starulong firstnumber;        /* Vertices are numbered starting from this. */
  arraypoolulong deadvertexcount;     /* Non-corner input vertices (killed). */
  arraypoolulong tetcount;                    /* Number of input tetrahedra. */
  unsigned int tetattribcount;      /* Number of attributes per tetrahedron. */
  arraypoolulong segmentcount;                  /* Number of input segments. */
  arraypoolulong facetcount;                      /* Number of input facets. */
  arraypoolulong holecount;                        /* Number of input holes. */
  arraypoolulong regioncount;                    /* Number of input regions. */
};

struct outputs {
  arraypoolulong vertexcount;
  arraypoolulong tetcount;
  arraypoolulong facecount;
  arraypoolulong boundaryfacecount;
  arraypoolulong edgecount;
};


void vertexcheckorientation(struct behavior *behave,
                            struct tetcomplex *plex,
                            tag vtx1,
                            tag vtx2,
                            tag vtx3,
                            tag vtx4)
{
  starreal ori;
  struct vertex *point1;
  struct vertex *point2;
  struct vertex *point3;
  struct vertex *point4;

  point1 = (struct vertex *) tetcomplextag2vertex(plex, vtx1);
  point2 = (struct vertex *) tetcomplextag2vertex(plex, vtx2);
  point3 = (struct vertex *) tetcomplextag2vertex(plex, vtx3);
  point4 = (struct vertex *) tetcomplextag2vertex(plex, vtx4);

  ori = orient3d(behave, point1->coord, point2->coord, point3->coord,
                 point4->coord);
  if (ori <= 0.0) {
    if (ori < 0.0) {
      printf("  !! !! Creating inverted tet\n");
    } else {
      printf("  !! !! Creating degenerate tet\n");
    }
    printf("        # %lu tag %lu (%.12g, %.12g, %.12g)"
           " # %lu tag %lu (%.12g, %.12g, %.12g)\n",
           (unsigned long) point1->number, (unsigned long) vtx1,
           (double) point1->coord[0], (double) point1->coord[1],
           (double) point1->coord[2],
           (unsigned long) point2->number, (unsigned long) vtx2,
           (double) point2->coord[0], (double) point2->coord[1],
           (double) point2->coord[2]);
    printf("        # %lu tag %lu (%.12g, %.12g, %.12g)"
           " # %lu tag %lu (%.12g, %.12g, %.12g)\n",
           (unsigned long) point3->number, (unsigned long) vtx3,
           (double) point3->coord[0], (double) point3->coord[1],
           (double) point3->coord[2],
           (unsigned long) point4->number, (unsigned long) vtx4,
           (double) point4->coord[0], (double) point4->coord[1],
           (double) point4->coord[2]);
  }
}

void vertexcheckpointintet(struct behavior *behave,
                           struct tetcomplex *plex,
                           tag vtx1,
                           tag vtx2,
                           tag vtx3,
                           tag vtx4,
                           tag invertex)
{
  struct vertex *point1;
  struct vertex *point2;
  struct vertex *point3;
  struct vertex *point4;
  struct vertex *inpoint;

  point1 = (struct vertex *) tetcomplextag2vertex(plex, vtx1);
  point2 = (struct vertex *) tetcomplextag2vertex(plex, vtx2);
  point3 = (struct vertex *) tetcomplextag2vertex(plex, vtx3);
  point4 = (struct vertex *) tetcomplextag2vertex(plex, vtx4);
  inpoint = (struct vertex *) tetcomplextag2vertex(plex, invertex);

  if ((orient3d(behave, point1->coord, point2->coord, point3->coord,
                inpoint->coord) < 0) ||
      (orient3d(behave, point1->coord, point2->coord, inpoint->coord,
                point4->coord) < 0) ||
      (orient3d(behave, point1->coord, inpoint->coord, point3->coord,
                point4->coord) < 0) ||
      (orient3d(behave, inpoint->coord, point2->coord, point3->coord,
                point4->coord) < 0)) {
    printf("  !! !! Point # %lu tag %lu (%.12g, %.12g, %.12g) not in tet",
           (unsigned long) inpoint->number, (unsigned long) invertex,
           (double) inpoint->coord[0], (double) inpoint->coord[1],
           (double) inpoint->coord[2]);
    printf("        # %lu tag %lu (%.12g, %.12g, %.12g)"
           " # %lu tag %lu (%.12g, %.12g, %.12g)\n",
           (unsigned long) point1->number, (unsigned long) vtx1,
           (double) point1->coord[0], (double) point1->coord[1],
           (double) point1->coord[2],
           (unsigned long) point2->number, (unsigned long) vtx2,
           (double) point2->coord[0], (double) point2->coord[1],
           (double) point2->coord[2]);
    printf("        # %lu tag %lu (%.12g, %.12g, %.12g)"
           " # %lu tag %lu (%.12g, %.12g, %.12g)\n",
           (unsigned long) point3->number, (unsigned long) vtx3,
           (double) point3->coord[0], (double) point3->coord[1],
           (double) point3->coord[2],
           (unsigned long) point4->number, (unsigned long) vtx4,
           (double) point4->coord[0], (double) point4->coord[1],
           (double) point4->coord[2]);
  }
}

int vertexcheckdelaunay(struct behavior *behave,
                        struct tetcomplex *plex,
                        tag vtx1,
                        tag vtx2,
                        tag vtx3,
                        tag adjacencies[2])
{
  struct vertex *point1;
  struct vertex *point2;
  struct vertex *point3;
  struct vertex *apex0;
  struct vertex *apex1;

  point1 = (struct vertex *) tetcomplextag2vertex(plex, vtx1);
  point2 = (struct vertex *) tetcomplextag2vertex(plex, vtx2);
  point3 = (struct vertex *) tetcomplextag2vertex(plex, vtx3);
  apex0 = (struct vertex *) tetcomplextag2vertex(plex, adjacencies[0]);
  apex1 = (struct vertex *) tetcomplextag2vertex(plex, adjacencies[1]);

  if (nonregular(behave, point1->coord, point2->coord, point3->coord,
                 apex1->coord, apex0->coord) > 0.0) {
    printf("  !! !! Non-Delaunay pair of tetrahedra.  Shared face is\n");
    printf("        # %lu tag %lu (%.12g, %.12g, %.12g)"
           " # %lu tag %lu (%.12g, %.12g, %.12g)"
           " # %lu tag %lu (%.12g, %.12g, %.12g)\n",
           (unsigned long) point1->number, (unsigned long) vtx1,
           (double) point1->coord[0], (double) point1->coord[1],
           (double) point1->coord[2],
           (unsigned long) point2->number, (unsigned long) vtx2,
           (double) point2->coord[0], (double) point2->coord[1],
           (double) point2->coord[2],
           (unsigned long) point3->number, (unsigned long) vtx3,
           (double) point3->coord[0], (double) point3->coord[1],
           (double) point3->coord[2]);
    printf("        Opposing vertices are\n");
    printf("        # %lu tag %lu (%.12g, %.12g, %.12g)"
           " # %lu tag %lu (%.12g, %.12g, %.12g)\n",
           (unsigned long) apex0->number, (unsigned long) vtx1,
           (double) apex0->coord[0], (double) apex0->coord[1],
           (double) apex0->coord[2],
           (unsigned long) apex1->number, (unsigned long) vtx3,
           (double) apex1->coord[0], (double) apex1->coord[1],
           (double) apex1->coord[2]);
    return 1;
  }
  return 0;
}

/*****************************************************************************/
/*                                                                           */
/*  inputtextline()   Read a nonempty line from a file.                      */
/*                                                                           */
/*  A line is considered "nonempty" if it contains something that looks like */
/*  a number.  Comments (prefaced by `#') are ignored.                       */
/*                                                                           */
/*****************************************************************************/

#ifndef STARLIBRARY

char *inputtextline(char *string,
                    FILE *infile,
                    char *infilename)
{
  char *result;

  /* Search for something that looks like a number. */
  do {
    result = fgets(string, INPUTLINESIZE, infile);
    if (result == (char *) NULL) {
      printf("  Error:  Unexpected end of file in %s.\n", infilename);
      starexit(1);
    }

    /* Skip anything that doesn't look like a number, a comment, */
    /*   or the end of a line.                                   */
    while ((*result != '\0') && (*result != '#') &&
           (*result != '.') && (*result != '+') && (*result != '-') &&
           ((*result < '0') || (*result > '9'))) {
      result++;
    }
  /* If it's a comment or end of line, read another line and try again. */
  } while ((*result == '#') || (*result == '\0'));

  return result;
}

#endif /* not STARLIBRARY */

/*****************************************************************************/
/*                                                                           */
/*  inputfindfield()   Find the next field of a string.                      */
/*                                                                           */
/*  Jumps past the current field by searching for whitespace, then jumps     */
/*  past the whitespace to find the next field.                              */
/*                                                                           */
/*****************************************************************************/

#ifndef STARLIBRARY

char *inputfindfield(char *string)
{
  char *result;

  result = string;
  /* Skip the current field.  Stop upon reaching whitespace. */
  while ((*result != '\0') && (*result != '#') &&
         (*result != ' ') && (*result != '\t')) {
    result++;
  }

  /* Now skip the whitespace and anything else that doesn't look like a */
  /*   number, a comment, or the end of a line.                         */
  while ((*result != '\0') && (*result != '#') &&
         (*result != '.') && (*result != '+') && (*result != '-') &&
         ((*result < '0') || (*result > '9'))) {
    result++;
  }

  /* Check for a comment (prefixed with `#'). */
  if (*result == '#') {
    *result = '\0';
  }

  return result;
}

#endif /* not STARLIBRARY */

/*****************************************************************************/
/*                                                                           */
/*  inputvertices()   Read the vertices from a file into a newly created     */
/*                    array, in `struct vertexshort' format.                 */
/*                                                                           */
/*****************************************************************************/

#ifndef STARLIBRARY

char *inputvertices(FILE *vertexfile,
                    char *vertexfilename,
                    struct inputs *in,
                    int markflag)
{
  char inputline[INPUTLINESIZE];
  char *stringptr;
  char *vertices;
  struct vertexshort *vertexptr;
  size_t vertexbytes;
  starlong firstnode;
  arraypoolulong i;
  unsigned int j;

  vertexbytes = sizeof(struct vertexshort) +
                (size_t) in->attribcount * sizeof(starreal);
  vertices = (char *) starmalloc((size_t) (in->vertexcount * vertexbytes));

  /* Read the vertices. */
  for (i = 0; i < in->vertexcount; i++) {
    vertexptr = (struct vertexshort *) &vertices[i * vertexbytes];

    stringptr = inputtextline(inputline, vertexfile, vertexfilename);
    if (i == 0) {
      firstnode = (starlong) strtol(stringptr, &stringptr, 0);
      if ((firstnode == 0) || (firstnode == 1)) {
        in->firstnumber = (starulong) firstnode;
      }
    }

    /* Read the vertex coordinates and attributes. */
    for (j = 0; j < 3 + in->attribcount; j++) {
      stringptr = inputfindfield(stringptr);
      if (*stringptr == '\0') {
        if (j >= 3) {
          vertexptr->coord[j] = 0.0;
        } else {
          printf("Error:  Vertex %lu has no %c coordinate.\n",
                 (unsigned long) (in->firstnumber + i),
                 (j == 0) ? 'x' : (j == 1) ? 'y' : 'z');
          starexit(1);
        }
      } else {
        vertexptr->coord[j] = (starreal) strtod(stringptr, &stringptr);
      }
    }

    if (markflag) {
      /* Read a vertex marker. */
      stringptr = inputfindfield(stringptr);
      if (*stringptr == '\0') {
        vertexptr->mark = 0;
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
      } else if (sizeof(vertexmarktype) > sizeof(long)) {
        vertexptr->mark = (vertexmarktype) strtoll(stringptr, &stringptr, 0);
#endif
      } else {
        vertexptr->mark = (vertexmarktype) strtol(stringptr, &stringptr, 0);
      }
    }
  }

  return vertices;
}

#endif /* not STARLIBRARY */

char *inputvertexfile(struct behavior *behave,
                      struct inputs *in,
                      FILE **polyfile)
{
  char inputline[INPUTLINESIZE];
  char *stringptr;
  FILE *infile;
  char *infilename;
  char *vertices;
  int mesh_dim;
  int markflag;

  if (behave->poly) {
    /* Read the vertices from a .poly file. */
    if (!behave->quiet) {
      printf("Opening %s.\n", behave->inpolyfilename);
    }

    *polyfile = fopen(behave->inpolyfilename, "r");
    if (*polyfile == (FILE *) NULL) {
      printf("  Error:  Cannot access file %s.\n", behave->inpolyfilename);
      starexit(1);
    }

    /* Read number of vertices, number of dimensions, number of vertex */
    /*   attributes, and number of boundary markers.                   */
    stringptr = inputtextline(inputline, *polyfile,
                              behave->inpolyfilename);
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
    if (sizeof(arraypoolulong) > sizeof(long)) {
      in->vertexcount = (arraypoolulong) strtoll(stringptr, &stringptr, 0);
    } else {
      in->vertexcount = (arraypoolulong) strtol(stringptr, &stringptr, 0);
    }
#else
    in->vertexcount = (arraypoolulong) strtol(stringptr, &stringptr, 0);
#endif
    stringptr = inputfindfield(stringptr);
    if (*stringptr == '\0') {
      mesh_dim = 3;
    } else {
      mesh_dim = (int) strtol(stringptr, &stringptr, 0);
    }
    stringptr = inputfindfield(stringptr);
    if (*stringptr == '\0') {
      in->attribcount = 0;
    } else {
      in->attribcount = (unsigned int) strtol(stringptr, &stringptr, 0);
    }
    stringptr = inputfindfield(stringptr);
    if (*stringptr == '\0') {
      markflag = 0;
    } else {
      markflag = (int) strtol(stringptr, &stringptr, 0);
    }

    if (in->vertexcount > 0) {
      infile = *polyfile;
      infilename = behave->inpolyfilename;
      behave->readnodefileflag = 0;
    } else {
      /* If the .poly file claims there are zero vertices, that means that */
      /*   the vertices should be read from a separate .node file.         */
      behave->readnodefileflag = 1;
      infilename = behave->innodefilename;
    }
  } else {
    behave->readnodefileflag = 1;
    infilename = behave->innodefilename;
    *polyfile = (FILE *) NULL;
  }
  in->deadvertexcount = 0;

  if (behave->readnodefileflag) {
    /* Read the vertices from a .node file. */
    if (!behave->quiet) {
      printf("Opening %s.\n", behave->innodefilename);
    }

    infile = fopen(behave->innodefilename, "r");
    if (infile == (FILE *) NULL) {
      printf("  Error:  Cannot access file %s.\n", behave->innodefilename);
      starexit(1);
    }

    /* Read number of vertices, number of dimensions, number of vertex */
    /*   attributes, and number of boundary markers.                   */
    stringptr = inputtextline(inputline, infile, behave->innodefilename);
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
    if (sizeof(arraypoolulong) > sizeof(long)) {
      in->vertexcount = (arraypoolulong) strtoll(stringptr, &stringptr, 0);
    } else {
      in->vertexcount = (arraypoolulong) strtol(stringptr, &stringptr, 0);
    }
#else
    in->vertexcount = (arraypoolulong) strtol(stringptr, &stringptr, 0);
#endif
    stringptr = inputfindfield(stringptr);
    if (*stringptr == '\0') {
      mesh_dim = 3;
    } else {
      mesh_dim = (int) strtol(stringptr, &stringptr, 0);
    }
    stringptr = inputfindfield(stringptr);
    if (*stringptr == '\0') {
      in->attribcount = 0;
    } else {
      in->attribcount = (unsigned int) strtol(stringptr, &stringptr, 0);
    }
    stringptr = inputfindfield(stringptr);
    if (*stringptr == '\0') {
      markflag = 0;
    } else {
      markflag = (int) strtol(stringptr, &stringptr, 0);
    }
  }

  if (in->vertexcount < 4) {
    printf("Error:  Input must have at least four input vertices.\n");
    starexit(1);
  }
  if (mesh_dim != 3) {
    printf("Error:  Star only works with three-dimensional meshes.\n");
    starexit(1);
  }

  in->firstnumber = behave->firstnumber;
  vertices = inputvertices(infile, infilename, in, markflag);

  if (behave->readnodefileflag) {
    fclose(infile);
  }

  behave->firstnumber = in->firstnumber;
  if (in->attribcount == 0) {
    behave->weighted = 0;
  }

  return vertices;
}

/*****************************************************************************/
/*                                                                           */
/*  inputvertexsort()   Sort an array of vertices (in `struct vertexshort'   */
/*                      format) along a z-order space-filling curve.         */
/*                                                                           */
/*  Uses quicksort.  Randomized O(n log n) time.                             */
/*                                                                           */
/*  Sorts whole `struct vertexshort' records, rather than pointers to        */
/*  records, because I think it will be faster for very large vertex sets,   */
/*  where quicksort's linear walk through the array favors the memory        */
/*  hierarchy better than pseudo-random record lookups.                      */
/*                                                                           */
/*****************************************************************************/

void inputvertexsort(char *vertices,
                     tag *vertextags,
                     arraypoolulong vertexcount,
                     size_t vertexbytes,
                     unsigned int attribcount)
{
  struct vertexshort tempvertex;
  struct vertexshort pivotvertex;
  struct vertexshort *leftptr, *rightptr, *pivotptr;
  arraypoolulong leftindex, rightindex, pivotindex;
  starreal pivotx, pivoty, pivotz;
  starreal tempreal;
  tag pivottag;
  tag temptag;
  unsigned int j;

  if (vertexcount == 2) {
    /* Base case of the recursion. */
    leftptr = (struct vertexshort *) vertices;
    rightptr = (struct vertexshort *) &vertices[vertexbytes];
    if (zorderbefore(rightptr->coord[0], rightptr->coord[1],
                     rightptr->coord[2], leftptr->coord[0],
                     leftptr->coord[1], leftptr->coord[2])) {
      temptag = vertextags[1];
      vertextags[1] = vertextags[0];
      vertextags[0] = temptag;
      tempvertex = *rightptr;
      *rightptr = *leftptr;
      *leftptr = tempvertex;
      for (j = 3; j < 3 + attribcount; j++) {
        tempreal = rightptr->coord[j];
        rightptr->coord[j] = leftptr->coord[j];
        leftptr->coord[j] = tempreal;
      }
    }
    return;
  }

  if (vertexcount < 5) {
    pivotindex = 0;
    pivotptr = (struct vertexshort *) vertices;
  } else {
    /* Choose a random pivot to split the array. */
    pivotindex = (arraypoolulong) randomnation((unsigned int) vertexcount);
    pivotptr = (struct vertexshort *) &vertices[pivotindex * vertexbytes];
  }
  pivottag = vertextags[pivotindex];
  pivotvertex = *pivotptr;
  pivotx = pivotvertex.coord[0];
  pivoty = pivotvertex.coord[1];
  pivotz = pivotvertex.coord[2];
  if (pivotindex > 0) {
    /* Move the first vertex of the array to take the pivot's place. */
    vertextags[pivotindex] = vertextags[0];
    *pivotptr = * (struct vertexshort *) vertices;
    for (j = 3; j < 3 + attribcount; j++) {
      /* Swap the pivot's attributes with the first vertex's. */
      tempreal = pivotptr->coord[j];
      pivotptr->coord[j] = ((struct vertexshort *) vertices)->coord[j];
      ((struct vertexshort *) vertices)->coord[j] = tempreal;
    }
  }

  /* Partition the array. */
  leftindex = 0;
  leftptr = (struct vertexshort *) vertices;
  rightindex = vertexcount;
  rightptr = (struct vertexshort *) &vertices[vertexcount * vertexbytes];
  do {
    /* Find a vertex whose z-order position is too large for the left. */
    do {
      leftindex++;
      leftptr = (struct vertexshort *) (((char *) leftptr) + vertexbytes);
    } while ((leftindex < rightindex) &&
             zorderbefore(leftptr->coord[0], leftptr->coord[1],
                          leftptr->coord[2], pivotx, pivoty, pivotz));
    /* Find a vertex whose z-order position is too small for the right. */
    do {
      rightindex--;
      rightptr = (struct vertexshort *) (((char *) rightptr) - vertexbytes);
    } while ((leftindex <= rightindex) &&
             zorderbefore(pivotx, pivoty, pivotz, rightptr->coord[0],
                          rightptr->coord[1], rightptr->coord[2]));

    if (leftindex < rightindex) {
      /* Swap the left and right vertices. */
      temptag = vertextags[rightindex];
      vertextags[rightindex] = vertextags[leftindex];
      vertextags[leftindex] = temptag;
      tempvertex = *rightptr;
      *rightptr = *leftptr;
      *leftptr = tempvertex;
      for (j = 3; j < 3 + attribcount; j++) {
        tempreal = rightptr->coord[j];
        rightptr->coord[j] = leftptr->coord[j];
        leftptr->coord[j] = tempreal;
      }
    }
  } while (leftindex < rightindex);

  /* Place the pivot in the middle of the partition by swapping with the */
  /*   right vertex.                                                     */
  vertextags[0] = vertextags[rightindex];
  vertextags[rightindex] = pivottag;
  * (struct vertexshort *) vertices = *rightptr;
  *rightptr = pivotvertex;
  for (j = 3; j < 3 + attribcount; j++) {
    /* Swap the right vertex's attributes with the pivot's. */
    tempreal = rightptr->coord[j];
    rightptr->coord[j] = ((struct vertexshort *) vertices)->coord[j];
    ((struct vertexshort *) vertices)->coord[j] = tempreal;
  }

  if (rightindex > 1) {
    /* Recursively sort the left subset. */
    inputvertexsort(vertices, vertextags, rightindex, vertexbytes,
                    attribcount);
  }
  if (rightindex < vertexcount - 2) {
    /* Recursively sort the right subset. */
    inputvertexsort(&vertices[(rightindex + 1) * vertexbytes],
                    &vertextags[rightindex + 1], vertexcount - rightindex - 1,
                    vertexbytes, attribcount);
  }
}

void inputvertexsort2(char *vertices,
                      tag *vertextags,
                      arraypoolulong vertexcount,
                      size_t vertexbytes,
                      unsigned int attribcount)
{
  struct vertexshort *leftptr, *rightptr, *pivotptr;
  arraypoolulong leftindex, rightindex, pivotindex;
  starreal pivotx, pivoty, pivotz;
  tag pivottag;
  tag temptag;

  if (vertexcount == 2) {
    /* Base case of the recursion. */
    leftptr = (struct vertexshort *) &vertices[vertextags[0] * vertexbytes];
    rightptr = (struct vertexshort *) &vertices[vertextags[1] * vertexbytes];
    if (zorderbefore(rightptr->coord[0], rightptr->coord[1],
                     rightptr->coord[2], leftptr->coord[0],
                     leftptr->coord[1], leftptr->coord[2])) {
      temptag = vertextags[1];
      vertextags[1] = vertextags[0];
      vertextags[0] = temptag;
    }
    return;
  }

  if (vertexcount < 5) {
    pivotindex = 0;
  } else {
    /* Choose a random pivot to split the array. */
    pivotindex = (arraypoolulong) randomnation((unsigned int) vertexcount);
  }
  pivottag = vertextags[pivotindex];
  pivotptr = (struct vertexshort *) &vertices[pivottag * vertexbytes];
  pivotx = pivotptr->coord[0];
  pivoty = pivotptr->coord[1];
  pivotz = pivotptr->coord[2];
  if (pivotindex > 0) {
    /* Move the first vertex of the array to take the pivot's place. */
    vertextags[pivotindex] = vertextags[0];
  }

  /* Partition the array. */
  leftindex = 0;
  rightindex = vertexcount;
  do {
    /* Find a vertex whose z-order position is too large for the left. */
    do {
      leftindex++;
      leftptr = (struct vertexshort *) &vertices[vertextags[leftindex] *
                                                 vertexbytes];
    } while ((leftindex < rightindex) &&
             zorderbefore(leftptr->coord[0], leftptr->coord[1],
                          leftptr->coord[2], pivotx, pivoty, pivotz));
    /* Find a vertex whose z-order position is too small for the right. */
    do {
      rightindex--;
      rightptr = (struct vertexshort *) &vertices[vertextags[rightindex] *
                                                  vertexbytes];
    } while ((leftindex <= rightindex) &&
             zorderbefore(pivotx, pivoty, pivotz, rightptr->coord[0],
                          rightptr->coord[1], rightptr->coord[2]));

    if (leftindex < rightindex) {
      /* Swap the left and right vertices. */
      temptag = vertextags[rightindex];
      vertextags[rightindex] = vertextags[leftindex];
      vertextags[leftindex] = temptag;
    }
  } while (leftindex < rightindex);

  /* Place the pivot in the middle of the partition by swapping with the */
  /*   right vertex.                                                     */
  vertextags[0] = vertextags[rightindex];
  vertextags[rightindex] = pivottag;

  if (rightindex > 1) {
    /* Recursively sort the left subset. */
    inputvertexsort2(vertices, vertextags, rightindex, vertexbytes,
                     attribcount);
  }
  if (rightindex < vertexcount - 2) {
    /* Recursively sort the right subset. */
    inputvertexsort2(vertices, &vertextags[rightindex + 1],
                     vertexcount - rightindex - 1, vertexbytes, attribcount);
  }
}

void inputverticesintopool(char *vertices,
                           struct inputs *in,
                           struct proxipool *pool)
{
  struct vertex *vertexptr;
  struct vertexshort *vertexshortptr;
  starreal *attributes;
  size_t vertexbytes;
  tag newtag;
  arraypoolulong i;
  unsigned int j;

  vertexshortptr = (struct vertexshort *) vertices;
  vertexbytes = sizeof(struct vertexshort) +
                (size_t) in->attribcount * sizeof(starreal);
  for (i = 0; i < in->vertexcount; i++) {
    newtag = proxipoolnew(pool, 0, (void **) &vertexptr);
    vertexptr->coord[0] = vertexshortptr->coord[0];
    vertexptr->coord[1] = vertexshortptr->coord[1];
    vertexptr->coord[2] = vertexshortptr->coord[2];
    vertexptr->mark = vertexshortptr->mark;
    vertexptr->number = in->vertextags[i];
    in->vertextags[i] = newtag;
    if (in->attribcount > 0) {
      attributes = (starreal *) proxipooltag2object2(pool, newtag);
      for (j = 0; j < in->attribcount; j++) {
        attributes[j] = vertexshortptr->coord[3 + j];
      }
    }

    vertexshortptr = (struct vertexshort *) (((char *) vertexshortptr) +
                                             vertexbytes);
  }
}

void inputverticesintopool2(char *vertices,
                            struct inputs *in,
                            struct proxipool *pool)
{
  struct vertex *vertexptr;
  struct vertexshort *vertexshortptr;
  starreal *attributes;
  size_t vertexbytes;
  tag newtag;
  arraypoolulong i;
  unsigned int j;

  vertexbytes = sizeof(struct vertexshort) +
                (size_t) in->attribcount * sizeof(starreal);

  for (i = 0; i < in->vertexcount; i++) {
    newtag = proxipoolnew(pool, 0, (void **) &vertexptr);
    vertexshortptr = (struct vertexshort *) &vertices[in->vertextags[i] *
                                                      vertexbytes];
    vertexptr->coord[0] = vertexshortptr->coord[0];
    vertexptr->coord[1] = vertexshortptr->coord[1];
    vertexptr->coord[2] = vertexshortptr->coord[2];
    vertexptr->mark = vertexshortptr->mark;
    vertexptr->number = in->vertextags[i];
    in->vertextags[i] = newtag;
    if (in->attribcount > 0) {
      attributes = (starreal *) proxipooltag2object2(pool, newtag);
      for (j = 0; j < in->attribcount; j++) {
        attributes[j] = vertexshortptr->coord[3 + j];
      }
    }
  }
}

void inputverticessortstore(char *vertices,
                            struct inputs *in,
                            struct proxipool *pool)
{
  arraypoolulong i;
  tag *tags;

  tags = (tag *) starmalloc((size_t) (in->vertexcount * sizeof(tag)));
  for (i = 0; i < in->vertexcount; i++) {
    tags[i] = (tag) (in->firstnumber + i);
  }

  inputvertexsort(vertices, tags, in->vertexcount,
                  sizeof(struct vertexshort) +
                  (size_t) in->attribcount * sizeof(starreal),
                  in->attribcount);

  in->vertextags = tags;
  inputverticesintopool(vertices, in, pool);
}

FILE *inputverticesreadsortstore(struct behavior *behave,
                                 struct inputs *in,
                                 struct proxipool *pool)
{
  char *vertices;
  FILE *polyfile;

  vertices = inputvertexfile(behave, in, &polyfile);
  proxipoolinit(pool, sizeof(struct vertex),
                (size_t) (in->attribcount * sizeof(starreal)),
                behave->verbose);
  inputverticessortstore(vertices, in, pool);
  starfree(vertices);

  return polyfile;
}

void inputmaketagmap(struct proxipool *vertexpool,
                     starulong firstnumber,
                     tag *vertextags)
{
  struct vertex *vertexptr;
  tag iterator;

  iterator = proxipooliterate(vertexpool, NOTATAG);
  while (iterator != NOTATAG) {
    vertexptr = proxipooltag2object(vertexpool, iterator);
    vertextags[vertexptr->number - firstnumber] = iterator;
    iterator = proxipooliterate(vertexpool, iterator);
  }
}

void inputtetrahedra(struct behavior *behave,
                     struct inputs *in,
                     struct proxipool *vertexpool,
                     struct outputs *out,
                     struct tetcomplex *plex)
{
  arraypoolulong corner[4];
  tag cornertag[4];
  char inputline[INPUTLINESIZE];
  char *stringptr;
  FILE *elefile;
  struct vertex *killvertex;
  tag killtag;
  arraypoolulong killnode;
  arraypoolulong elementnumber;
  arraypoolulong boundaryfacecount;
  int elemnodes;
  int result;
  int i;

  /* Read the tetrahedra from an .ele file. */
  if (!behave->quiet) {
    printf("Opening %s.\n", behave->inelefilename);
  }

  elefile = fopen(behave->inelefilename, "r");
  if (elefile == (FILE *) NULL) {
    printf("  Error:  Cannot access file %s.\n", behave->inelefilename);
    starexit(1);
  }

  /* Read number of tetrahedra, number of vertices per tetrahedron, and */
  /*   number of tetrahedron attributes from .ele file.                 */
  stringptr = inputtextline(inputline, elefile, behave->inelefilename);
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
  if (sizeof(arraypoolulong) > sizeof(long)) {
    in->tetcount = (arraypoolulong) strtoll(stringptr, &stringptr, 0);
  } else {
    in->tetcount = (arraypoolulong) strtol(stringptr, &stringptr, 0);
  }
#else
  in->tetcount = (arraypoolulong) strtol(stringptr, &stringptr, 0);
#endif
  stringptr = inputfindfield(stringptr);
  if (*stringptr == '\0') {
    elemnodes = 4;
  } else {
    elemnodes = (int) strtol(stringptr, &stringptr, 0);
    if (elemnodes < 4) {
      printf("Error:  Tetrahedra in %s must have at least 4 vertices.\n",
             behave->inelefilename);
      starexit(1);
    }
  }
  stringptr = inputfindfield(stringptr);
  if (*stringptr == '\0') {
    in->tetattribcount = 0;
  } else {
    in->tetattribcount = (unsigned int) strtol(stringptr, &stringptr, 0);
  }

  tetcomplexinit(plex, vertexpool, behave->verbose);

  if (!behave->quiet) {
    printf("Reconstructing mesh.\n");
  }

  boundaryfacecount = 0;
  for (elementnumber = behave->firstnumber;
       elementnumber < behave->firstnumber + in->tetcount; elementnumber++) {
    /* Read the tetrahedron's four vertices. */
    stringptr = inputtextline(inputline, elefile, behave->inelefilename);
    for (i = 0; i < 4; i++) {
      stringptr = inputfindfield(stringptr);
      if (*stringptr == '\0') {
        printf("Error:  Tetrahedron %lu is missing vertex %d in %s.\n",
               (unsigned long) elementnumber, i + 1, behave->inelefilename);
        starexit(1);
      } else {
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
        if (sizeof(arraypoolulong) > sizeof(long)) {
          corner[i] = (arraypoolulong) strtoll(stringptr, &stringptr, 0);
        } else {
          corner[i] = (arraypoolulong) strtol(stringptr, &stringptr, 0);
        }
#else
        corner[i] = (arraypoolulong) strtol(stringptr, &stringptr, 0);
#endif
        if ((corner[i] < behave->firstnumber) ||
            (corner[i] >= behave->firstnumber + in->vertexcount)) {
          printf("Error:  Tetrahedron %lu has an invalid vertex index.\n",
                 (unsigned long) elementnumber);
          starexit(1);
        }
        cornertag[i] = in->vertextags[corner[i] - behave->firstnumber];
      }
    }

    /* Find out about (and throw away) extra nodes. */
    for (i = 4; i < elemnodes; i++) {
      stringptr = inputfindfield(stringptr);
      if (*stringptr != '\0') {
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
        if (sizeof(arraypoolulong) > sizeof(long)) {
          killnode = (arraypoolulong) strtoll(stringptr, &stringptr, 0);
        } else {
          killnode = (arraypoolulong) strtol(stringptr, &stringptr, 0);
        }
#else
        killnode = (arraypoolulong) strtol(stringptr, &stringptr, 0);
#endif
        if ((killnode >= behave->firstnumber) &&
            (killnode < behave->firstnumber + in->vertexcount)) {
          /* Delete the non-corner node if it's not already deleted. */
          killtag = in->vertextags[killnode - behave->firstnumber];
          killvertex = (struct vertex *) proxipooltag2object(vertexpool,
                                                             killtag);
          if (killvertex->number != DEADVERTEX) {
            killvertex->number = DEADVERTEX;
            proxipoolfree(vertexpool, killtag);
            in->deadvertexcount++;
          }
        }
      }
    }

#ifdef SELF_CHECK
    vertexcheckorientation(behave, plex, cornertag[0], cornertag[1],
                           cornertag[2], cornertag[3]);
#endif /* SELF_CHECK */
    result = tetcomplexinserttet(plex, cornertag[0], cornertag[1],
                                 cornertag[2], cornertag[3]);
    if (result > 0) {
      boundaryfacecount += (result - 6);
    }
  }

  fclose(elefile);

  out->vertexcount = in->vertexcount - in->deadvertexcount;
  out->tetcount = tetcomplextetcount(plex);
  out->boundaryfacecount = boundaryfacecount;
  out->facecount = 2 * out->tetcount + (out->boundaryfacecount / 2);
  out->edgecount = out->vertexcount + out->facecount - out->tetcount - 1;
}

int inputsegmentheader(struct behavior *behave,
                       struct inputs *in,
                       FILE *polyfile)
{
  char inputline[INPUTLINESIZE];
  char *stringptr;
  int markflag;

  /* Read number of segments and number of boundary markers from .poly file. */
  stringptr = inputtextline(inputline, polyfile, behave->inpolyfilename);
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
  if (sizeof(arraypoolulong) > sizeof(long)) {
    in->segmentcount = (arraypoolulong) strtoll(stringptr, &stringptr, 0);
  } else {
    in->segmentcount = (arraypoolulong) strtol(stringptr, &stringptr, 0);
  }
#else
  in->segmentcount = (arraypoolulong) strtol(stringptr, &stringptr, 0);
#endif

  stringptr = inputfindfield(stringptr);
  if (*stringptr == '\0') {
    markflag = 0;
  } else {
    markflag = (int) strtol(stringptr, &stringptr, 0);
  }
  return markflag;
}

void inputsegment(struct behavior *behave,
                  struct inputs *in,
                  FILE *polyfile,
                  arraypoolulong segnumber,
                  int markflag,
                  struct segment *seg)
{
  char inputline[INPUTLINESIZE];
  char *stringptr;
  arraypoolulong endpoint;

  seg->number = behave->firstnumber + segnumber;
  /* Read a segment from a .poly file. */
  stringptr = inputtextline(inputline, polyfile, behave->inpolyfilename);
  stringptr = inputfindfield(stringptr);
  if (*stringptr == '\0') {
    printf("Error:  Segment %ld has no endpoints in %s.\n",
           (long) (behave->firstnumber + segnumber), behave->inpolyfilename);
    exit(1);
  } else {
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
    if (sizeof(arraypoolulong) > sizeof(long)) {
      endpoint = (arraypoolulong) strtoll(stringptr, &stringptr, 0);
    } else {
      endpoint = (arraypoolulong) strtol(stringptr, &stringptr, 0);
    }
#else
    endpoint = (arraypoolulong) strtol(stringptr, &stringptr, 0);
#endif
  }
  if ((endpoint < behave->firstnumber) ||
      (endpoint >= behave->firstnumber + in->vertexcount)) {
    if (!behave->quiet) {
      printf("Warning:  Invalid first endpoint of segment %ld in %s.\n",
             (long) (behave->firstnumber + segnumber), behave->inpolyfilename);
    }
  } else {
    seg->endpoint[0] = in->vertextags[endpoint];
  }

  stringptr = inputfindfield(stringptr);
  if (*stringptr == '\0') {
    printf("Error:  Segment %ld is missing its second endpoint in %s.\n",
           (long) (behave->firstnumber + segnumber), behave->inpolyfilename);
    exit(1);
  } else {
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
    if (sizeof(arraypoolulong) > sizeof(long)) {
      endpoint = (arraypoolulong) strtoll(stringptr, &stringptr, 0);
    } else {
      endpoint = (arraypoolulong) strtol(stringptr, &stringptr, 0);
    }
#else
    endpoint = (arraypoolulong) strtol(stringptr, &stringptr, 0);
#endif
  }
  if ((endpoint < behave->firstnumber) ||
      (endpoint >= behave->firstnumber + in->vertexcount)) {
    if (!behave->quiet) {
      printf("Warning:  Invalid second endpoint of segment %ld in %s.\n",
             (long) (behave->firstnumber + segnumber), behave->inpolyfilename);
    }
  } else {
    seg->endpoint[1] = in->vertextags[endpoint];
  }

  seg->mark = 0;
  if (markflag) {
    stringptr = inputfindfield(stringptr);
    if (*stringptr != '\0') {
      seg->mark = (vertexmarktype) strtol(stringptr, &stringptr, 0);
    }
  }
}

/**                                                                         **/
/**                                                                         **/
/********* Input routines end here                                   *********/


