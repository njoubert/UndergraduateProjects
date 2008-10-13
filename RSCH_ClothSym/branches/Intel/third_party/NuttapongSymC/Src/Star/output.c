/********* Output routines begin here                                *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  outputfilefinish()   Write the command line to the output file so the    */
/*                       user can remember how the file was generated.       */
/*                       Close the file.                                     */
/*                                                                           */
/*****************************************************************************/

#ifndef STARLIBRARY

void outputfilefinish(FILE *outfile,
                      int argc,
                      char **argv)
{
  int i;

  fputs("# Generated by", outfile);
  for (i = 0; i < argc; i++) {
    putc(' ', outfile);
    fputs(argv[i], outfile);
  }
  putc('\n', outfile);
  fclose(outfile);
}

#endif /* not PYRLIBRARY */

/*****************************************************************************/
/*                                                                           */
/*  outputpreparevertices()   Determines which vertices are in the mesh, and */
/*                            marks the vertices on the hull boundary.       */
/*                                                                           */
/*  If the -j switch is selected (behave->jettison), the vertices in the     */
/*  mesh are identified and their "number" fields are set to ACTIVEVERTEX.   */
/*                                                                           */
/*  If the -B switch is not selected (behave->nobound), every vertex on the  */
/*  boundary of the convex hull whose mark is zero has its mark set to 1.    */
/*                                                                           */
/*  behave:  Command line switches, operation counts, etc.                   */
/*  vertexpool:  The proxipool of vertices associated with 'plex'.           */
/*  plex:  The tetcomplex in question.                                       */
/*                                                                           */
/*****************************************************************************/

void outputpreparevertices(struct behavior *behave,
                           struct tetcomplex *plex)
{
  struct tetcomplexposition position;
  struct proxipool *pool;
  struct vertex *vertexptr0;
  struct vertex *vertexptr1;
  struct vertex *vertexptr2;
  struct vertex *vertexptr3;
  tag tet[4];

  if (behave->jettison) {
    if (behave->verbose) {
      if (behave->nobound) {
        printf("  Identifying vertices in mesh.\n");
      } else {
        printf("  Identifying vertices in mesh and "
               "marking boundary vertices.\n");
      }
    }
  } else {
    if (behave->nobound) {
      return;
    } else if (behave->verbose) {
      printf("  Marking boundary vertices.\n");
    }
  }

  pool = plex->vertexpool;
  /* Iterate through all the tetrahedra in the complex. */
  tetcomplexiteratorinit(plex, &position);
  tetcomplexiterate(&position, tet);
  while (tet[0] != STOP) {
    if ((tet[2] == GHOSTVERTEX) || (tet[3] == GHOSTVERTEX)) {
      if (!behave->nobound) {
        vertexptr0 = (struct vertex *) proxipooltag2object(pool, tet[0]);
        vertexptr1 = (struct vertex *) proxipooltag2object(pool, tet[1]);
        vertexptr2 = (struct vertex *)
          proxipooltag2object(pool, tet[(tet[2] == GHOSTVERTEX) ? 3 : 2]);
        /* These vertices are on the boundary of the triangulation.  If any */
        /*   of them has a vertex mark of zero, change it to one.           */
        if (vertexptr0->mark == 0) {
          vertexptr0->mark = 1;
        }
        if (vertexptr1->mark == 0) {
          vertexptr1->mark = 1;
        }
        if (vertexptr2->mark == 0) {
          vertexptr2->mark = 1;
        }
      }
    } else if (behave->jettison) {
      vertexptr0 = (struct vertex *) proxipooltag2object(pool, tet[0]);
      vertexptr1 = (struct vertex *) proxipooltag2object(pool, tet[1]);
      vertexptr2 = (struct vertex *) proxipooltag2object(pool, tet[2]);
      vertexptr3 = (struct vertex *) proxipooltag2object(pool, tet[3]);
      vertexptr0->number = ACTIVEVERTEX;
      vertexptr1->number = ACTIVEVERTEX;
      vertexptr2->number = ACTIVEVERTEX;
      vertexptr3->number = ACTIVEVERTEX;
    }

    tetcomplexiterate(&position, tet);
  }
}

/*****************************************************************************/
/*                                                                           */
/*  outputvertices()   Write the vertices to a .node file.                   */
/*                                                                           */
/*  in:  Properties of the input geometry.                                   */
/*                                                                           */
/*****************************************************************************/

void outputvertices(struct behavior *behave,
                    struct inputs *in,
                    struct proxipool *pool,
                    arraypoolulong vertexcount,
                    int argc,
                    char **argv)
{
  struct vertex *vertexptr;
  starreal *attributes;
  FILE *outfile;
  tag vertextag;
  arraypoolulong vertexnumber;
  arraypoolulong i;
  unsigned int j;

  if (!behave->quiet) {
    printf("Writing %s.\n", behave->outnodefilename);
  }

  outfile = fopen(behave->outnodefilename, "w");
  if (outfile == (FILE *) NULL) {
    printf("  Error:  Cannot create file %s.\n", behave->outnodefilename);
    starexit(1);
  }

  if (!behave->jettison) {
    vertexcount = proxipoolobjects(pool);
  }

  /* Number of vertices, number of dimensions, number of vertex attributes, */
  /*   and number of boundary markers (zero or one).                        */
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
  if (sizeof(arraypoolulong) > sizeof(long)) {
    fprintf(outfile, "%llu  3  %u  %d\n", (unsigned long long) vertexcount,
            in->attribcount, 1 - behave->nobound);
  } else {
    fprintf(outfile, "%lu  3  %u  %d\n", (unsigned long) vertexcount,
            in->attribcount, 1 - behave->nobound);
  }
#else
  fprintf(outfile, "%lu  3  %u  %d\n", (unsigned long) vertexcount,
          in->attribcount, 1 - behave->nobound);
#endif

  vertexnumber = (arraypoolulong) behave->firstnumber;
  vertextag = proxipooliterate(pool, NOTATAG);
  i = 0;
  while ((behave->jettison && (vertextag != NOTATAG)) ||
         (!behave->jettison && (i < in->vertexcount))) {
    if (!behave->jettison) {
      vertextag = in->vertextags[i];
    }
    vertexptr = (struct vertex *) proxipooltag2object(pool, vertextag);

    if ((vertexptr->number != DEADVERTEX) &&
        (!behave->jettison || (vertexptr->number == ACTIVEVERTEX))) {
      /* Node number, x, y, and z coordinates. */
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
      if (sizeof(arraypoolulong) > sizeof(long)) {
        fprintf(outfile, "%4llu    %.17g  %.17g  %.17g",
                (unsigned long long) vertexnumber,
                (double) vertexptr->coord[0], (double) vertexptr->coord[1],
                (double) vertexptr->coord[2]);
      } else {
        fprintf(outfile, "%4lu    %.17g  %.17g  %.17g",
                (unsigned long) vertexnumber,
                (double) vertexptr->coord[0], (double) vertexptr->coord[1],
                (double) vertexptr->coord[2]);
      }
#else
      fprintf(outfile, "%4lu    %.17g  %.17g  %.17g",
              (unsigned long) vertexnumber,
              (double) vertexptr->coord[0], (double) vertexptr->coord[1],
              (double) vertexptr->coord[2]);
#endif
      if (in->attribcount > 0) {
        attributes = (starreal *) proxipooltag2object2(pool,
                                                       in->vertextags[i]);
        for (j = 0; j < in->attribcount; j++) {
          /* Write an attribute. */
          fprintf(outfile, "  %.17g", (double) attributes[j]);
        }
      }
      if (behave->nobound) {
        putc('\n', outfile);
      } else {
        /* Write the boundary marker. */
        fprintf(outfile, "    %ld\n", (long) vertexptr->mark);
      }

      vertexptr->number = vertexnumber;
      vertexnumber++;
    }

    if (behave->jettison) {
      vertextag = proxipooliterate(pool, vertextag);
    } else {
      i++;
    }
  }

  outputfilefinish(outfile, argc, argv);
}

void outputnumbervertices(struct behavior *behave,
                          struct inputs *in,
                          struct proxipool *pool)
{
  struct vertex *vertexptr;
  tag vertextag;
  arraypoolulong vertexnumber;
  arraypoolulong i;

  vertexnumber = (arraypoolulong) behave->firstnumber;
  if (behave->jettison) {
    vertextag = proxipooliterate(pool, NOTATAG);
    while (vertextag != NOTATAG) {
      vertexptr = (struct vertex *) proxipooltag2object(pool, vertextag);
      if (vertexptr->number == ACTIVEVERTEX) {
        vertexptr->number = vertexnumber;
        vertexnumber++;
      }
      vertextag = proxipooliterate(pool, vertextag);
    }
  } else {
    for (i = 0; i < in->vertexcount; i++) {
      vertexptr = (struct vertex *) proxipooltag2object(pool,
                                                        in->vertextags[i]);
      if (vertexptr->number != DEADVERTEX) {
        vertexptr->number = vertexnumber;
        vertexnumber++;
      }
    }
  }
}

/*****************************************************************************/
/*                                                                           */
/*  outputtetrahedra()   Write the tetrahedra to an .ele file.               */
/*                                                                           */
/*****************************************************************************/

void outputtetrahedra(struct behavior *behave,
                      struct inputs *in,
                      struct tetcomplex *plex,
                      int argc,
                      char **argv)
{
  struct tetcomplexposition position;
  struct proxipool *pool;
  struct vertex *vertexptr0;
  struct vertex *vertexptr1;
  struct vertex *vertexptr2;
  struct vertex *vertexptr3;
  tag tet[4];
  FILE *outfile;
  arraypoolulong tetnumber;

  if (!behave->quiet) {
    printf("Writing %s.\n", behave->outelefilename);
  }

  outfile = fopen(behave->outelefilename, "w");
  if (outfile == (FILE *) NULL) {
    printf("  Error:  Cannot create file %s.\n", behave->outelefilename);
    starexit(1);
  }

  /* Number of tetrahedra, vertices per tetrahedron, attributes */
  /*   per tetrahedron.                                         */
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
  if (sizeof(arraypoolulong) > sizeof(long)) {
    fprintf(outfile, "%llu  %d  0\n",
            (unsigned long long) tetcomplextetcount(plex),
            (behave->order + 1) * (behave->order + 2) * (behave->order + 3) /
            6);
  } else {
    fprintf(outfile, "%lu  %d  0\n",
            (unsigned long) tetcomplextetcount(plex),
            (behave->order + 1) * (behave->order + 2) * (behave->order + 3) /
            6);
  }
#else
  fprintf(outfile, "%lu  %d  0\n", (unsigned long) tetcomplextetcount(plex),
          (behave->order + 1) * (behave->order + 2) * (behave->order + 3) / 6);
#endif

  pool = plex->vertexpool;
  tetnumber = (arraypoolulong) behave->firstnumber;

  /* Iterate through all the (non-ghost) tetrahedra in the complex. */
  tetcomplexiteratorinit(plex, &position);
  tetcomplexiteratenoghosts(&position, tet);
  while (tet[0] != STOP) {
    vertexptr0 = (struct vertex *) proxipooltag2object(pool, tet[0]);
    vertexptr1 = (struct vertex *) proxipooltag2object(pool, tet[1]);
    vertexptr2 = (struct vertex *) proxipooltag2object(pool, tet[2]);
    vertexptr3 = (struct vertex *) proxipooltag2object(pool, tet[3]);

    /* Tetrahedron number and indices for four vertices. */
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
    if (sizeof(arraypoolulong) > sizeof(long)) {
      fprintf(outfile, "%4llu    %4llu  %4llu  %4llu  %4llu\n",
              (unsigned long long) tetnumber,
              (unsigned long long) vertexptr0->number,
              (unsigned long long) vertexptr1->number,
              (unsigned long long) vertexptr2->number,
              (unsigned long long) vertexptr3->number);
    } else {
      fprintf(outfile, "%4lu    %4lu  %4lu  %4lu  %4lu\n",
              (unsigned long) tetnumber,
              (unsigned long) vertexptr0->number,
              (unsigned long) vertexptr1->number,
              (unsigned long) vertexptr2->number,
              (unsigned long) vertexptr3->number);
    }
#else
    fprintf(outfile, "%4lu    %4lu  %4lu  %4lu  %4lu\n",
            (unsigned long) tetnumber,
            (unsigned long) vertexptr0->number,
            (unsigned long) vertexptr1->number,
            (unsigned long) vertexptr2->number,
            (unsigned long) vertexptr3->number);
#endif

    tetcomplexiteratenoghosts(&position, tet);
    tetnumber++;
  }

  outputfilefinish(outfile, argc, argv);
}

/*****************************************************************************/
/*                                                                           */
/*  outputedges()   Write the edges to an .edge file.                        */
/*                                                                           */
/*****************************************************************************/

void outputedges(struct behavior *behave,
                 struct inputs *in,
                 struct tetcomplex *plex,
                 arraypoolulong edgecount,
                 int argc,
                 char **argv)
{
  struct tetcomplexposition position;
  struct proxipool *pool;
  struct vertex *originptr;
  struct vertex *destinptr;
  tag tet[4];
  tag adjacencies[2];
  FILE *outfile;
  tag origin;
  tag destin;
  tag apex;
  tag stopvtx;
  tag searchvtx;
  tag swaptag;
  arraypoolulong edgenumber;
  int writeflag;
  int i;

  if (!behave->quiet) {
    printf("Writing %s.\n", behave->edgefilename);
  }

  outfile = fopen(behave->edgefilename, "w");
  if (outfile == (FILE *) NULL) {
    printf("  Error:  Cannot create file %s.\n", behave->edgefilename);
    starexit(1);
  }

  /* Number of edges, number of boundary markers (zero or one). */
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
  if (sizeof(arraypoolulong) > sizeof(long)) {
    fprintf(outfile, "%llu  %d\n", (unsigned long long) edgecount,
            1 - behave->nobound);
  } else {
    fprintf(outfile, "%lu  %d\n", (unsigned long) edgecount,
            1 - behave->nobound);
  }
#else
  fprintf(outfile, "%lu  %d\n", (unsigned long) edgecount,
          1 - behave->nobound);
#endif

  pool = plex->vertexpool;
  edgenumber = (arraypoolulong) behave->firstnumber;

  /* Iterate through all the (non-ghost) tetrahedra in the complex. */
  tetcomplexiteratorinit(plex, &position);
  tetcomplexiteratenoghosts(&position, tet);
  while (tet[0] != STOP) {
    /* Look at all six edges of the tetrahedron. */
    for (i = 0; i < 6; i++) {
      if (tet[0] < tet[1]) {
        origin = tet[0];
        destin = tet[1];
        apex = tet[2];
        stopvtx = tet[3];
      } else {
        origin = tet[1];
        destin = tet[0];
        apex = tet[3];
        stopvtx = tet[2];
      }

      searchvtx = apex;
      writeflag = 1;
      do {
        if (!tetcomplexadjacencies(plex, origin, destin, searchvtx,
                                   adjacencies)) {
          printf("Internal error in outputedges():\n");
          printf("  Complex returned tetrahedron that can't be queried.\n");
          internalerror();
        }
        if (adjacencies[0] == GHOSTVERTEX) {
          writeflag = searchvtx == apex;
        }
        searchvtx = adjacencies[0];
        if (searchvtx < apex) {
          writeflag = 0;
        }
      } while (writeflag &&
               (searchvtx != stopvtx) && (searchvtx != GHOSTVERTEX));

      if (writeflag) {
        originptr = (struct vertex *) proxipooltag2object(pool, origin);
        destinptr = (struct vertex *) proxipooltag2object(pool, destin);

        /* Edge number and indices for two vertices. */
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
        if (sizeof(arraypoolulong) > sizeof(long)) {
          fprintf(outfile, "%4llu    %4llu  %4llu",
                  (unsigned long long) edgenumber,
                  (unsigned long long) originptr->number,
                  (unsigned long long) destinptr->number);
        } else {
          fprintf(outfile, "%4lu    %4lu  %4lu",
                  (unsigned long) edgenumber,
                  (unsigned long) originptr->number,
                  (unsigned long) destinptr->number);
        }
#else
        fprintf(outfile, "%4lu    %4lu  %4lu",
                (unsigned long) edgenumber,
                (unsigned long) originptr->number,
                (unsigned long) destinptr->number);
#endif
        if (behave->nobound) {
          putc('\n', outfile);
        } else if (searchvtx == GHOSTVERTEX) {
          fputs("  1\n", outfile);
        } else {
          fputs("  0\n", outfile);
        }

        edgenumber++;
      }

      /* The following shift cycles (tet[0], tet[1]) through all the edges   */
      /*   while maintaining the tetrahedron's orientation.  The schedule is */
      /*   i = 0:  0 1 2 3 => 1 2 0 3                                        */
      /*   i = 1:  1 2 0 3 => 1 3 2 0                                        */
      /*   i = 2:  1 3 2 0 => 3 2 1 0                                        */
      /*   i = 3:  3 2 1 0 => 3 0 2 1                                        */
      /*   i = 4:  3 0 2 1 => 0 2 3 1                                        */
      /*   i = 5:  0 2 3 1 => 0 1 2 3 (which isn't used).                    */
      if ((i & 1) == 0) {
        swaptag = tet[0];
        tet[0] = tet[1];
        tet[1] = tet[2];
        tet[2] = swaptag;
      } else {
        swaptag = tet[3];
        tet[3] = tet[2];
        tet[2] = tet[1];
        tet[1] = swaptag;
      }
    }

    tetcomplexiteratenoghosts(&position, tet);
  }

  outputfilefinish(outfile, argc, argv);
}

/*****************************************************************************/
/*                                                                           */
/*  outputfaces()   Write the triangular faces to a .face file.              */
/*                                                                           */
/*****************************************************************************/

void outputfaces(struct behavior *behave,
                 struct inputs *in,
                 struct tetcomplex *plex,
                 arraypoolulong facecount,
                 int argc,
                 char **argv)
{
  struct tetcomplexposition position;
  struct proxipool *pool;
  struct vertex *vertexptr0;
  struct vertex *vertexptr1;
  struct vertex *vertexptr2;
  tag tet[4];
  tag adjacencies[2];
  FILE *outfile;
  arraypoolulong facenumber;
  int i;

  if (!behave->quiet) {
    printf("Writing %s.\n", behave->facefilename);
  }

  outfile = fopen(behave->facefilename, "w");
  if (outfile == (FILE *) NULL) {
    printf("  Error:  Cannot create file %s.\n", behave->facefilename);
    starexit(1);
  }

  /* Number of triangular faces, number of boundary markers (zero or one). */
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
  if (sizeof(arraypoolulong) > sizeof(long)) {
    fprintf(outfile, "%llu  %d\n", (unsigned long long) facecount,
            1 - behave->nobound);
  } else {
    fprintf(outfile, "%lu  %d\n", (unsigned long) facecount,
            1 - behave->nobound);
  }
#else
  fprintf(outfile, "%lu  %d\n", (unsigned long) facecount,
          1 - behave->nobound);
#endif

  pool = plex->vertexpool;
  facenumber = (arraypoolulong) behave->firstnumber;

  /* Iterate through all the (non-ghost) tetrahedra in the complex. */
  tetcomplexiteratorinit(plex, &position);
  tetcomplexiteratenoghosts(&position, tet);
  while (tet[0] != STOP) {
    /* Look at all four faces of the tetrahedron. */
    for (i = 0; i < 4; i++) {
      if (!tetcomplexadjacencies(plex, tet[i ^ 1], tet[i ^ 2], tet[i ^ 3],
                                 adjacencies)) {
        printf("Internal error in outputfaces():\n");
        printf("  Iterator returned tetrahedron that can't be queried.\n");
        internalerror();
      }

      /* `adjacencies[1]' is the apex of the tetrahedron adjoining this   */
      /*   tetrahedron on the face (tet[i ^ 1], tet[i ^ 2], tet[i ^ 3]).  */
      /*   So that each face is written only once, write the face if this */
      /*   tetrahedron's apex tag is smaller than the neighbor's.  (Note  */
      /*   that the ghost tetrahedron has the largest tag of all.)        */
      if (tet[i] < adjacencies[1]) {
        /* The vertices of the face are written in counterclockwise order */
        /*   as viewed from _outside_ this tetrahedron.                   */
        vertexptr0 = (struct vertex *) proxipooltag2object(pool, tet[i ^ 3]);
        vertexptr1 = (struct vertex *) proxipooltag2object(pool, tet[i ^ 2]);
        vertexptr2 = (struct vertex *) proxipooltag2object(pool, tet[i ^ 1]);

        /* Face number and indices for three vertices. */
#if (defined(LLONG_MAX) || defined(LONG_LONG_MAX) || defined(LONGLONG_MAX))
        if (sizeof(arraypoolulong) > sizeof(long)) {
          fprintf(outfile, "%4llu    %4llu  %4llu  %4llu",
                  (unsigned long long) facenumber,
                  (unsigned long long) vertexptr0->number,
                  (unsigned long long) vertexptr1->number,
                  (unsigned long long) vertexptr2->number);
        } else {
          fprintf(outfile, "%4lu    %4lu  %4lu  %4lu",
                  (unsigned long) facenumber,
                  (unsigned long) vertexptr0->number,
                  (unsigned long) vertexptr1->number,
                  (unsigned long) vertexptr2->number);
        }
#else
        fprintf(outfile, "%4lu    %4lu  %4lu  %4lu",
                (unsigned long) facenumber,
                (unsigned long) vertexptr0->number,
                (unsigned long) vertexptr1->number,
                (unsigned long) vertexptr2->number);
#endif
        if (behave->nobound) {
          putc('\n', outfile);
        } else if (adjacencies[1] == GHOSTVERTEX) {
          fputs("  1\n", outfile);
        } else {
          fputs("  0\n", outfile);
        }

        facenumber++;
      }
    }

    tetcomplexiteratenoghosts(&position, tet);
  }

  outputfilefinish(outfile, argc, argv);
}

/**                                                                         **/
/**                                                                         **/
/********* Output routines end here                                  *********/


