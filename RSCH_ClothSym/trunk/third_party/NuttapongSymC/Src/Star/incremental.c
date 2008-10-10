/********* Incremental insertion Delaunay routines begin here        *********/
/**                                                                         **/
/**                                                                         **/

#define BRIOSAMPLERATE 10

void incrementalbrio(struct inputs *in)
{
  arraypoolulong levelvertexcount[256];
  arraypoolulong levelvertexindex[256];
  tag *vertextags;
  tag *temptags;
  arraypoolulong count, nextcount;
  arraypoolulong tempsize;
  arraypoolulong i;
  int level;

  level = 0;
  count = in->vertexcount;
  while (count > 0) {
    nextcount = count / BRIOSAMPLERATE;
    levelvertexcount[level] = count;
    levelvertexindex[level] = ((level & 1) == 0) ? count - 1: nextcount;
    count = nextcount;
    level++;
  }
  levelvertexcount[level] = 0;

  tempsize = levelvertexcount[1];
  if (tempsize > 0) {
    vertextags = in->vertextags;
    temptags = (tag *) starmalloc((size_t) (tempsize * sizeof(tag)));

    i = in->vertexcount;
    while (i > 0) {
      i--;
      level = 0;
      while (randomnation((unsigned long) levelvertexcount[level]) <
             levelvertexcount[level + 1]) {
        levelvertexcount[level]--;
        level++;
      }
      levelvertexcount[level]--;
      if (level == 0) {
        vertextags[levelvertexindex[0]] = vertextags[i];
      } else {
        temptags[levelvertexindex[level]] = vertextags[i];
      }
      if ((level & 1) == 0) {
        levelvertexindex[level]--;
      } else {
        levelvertexindex[level]++;
      }
    }

    for (i = 0; i < tempsize; i++) {
      vertextags[i] = temptags[i];
    }

    starfree(temptags);
  }
}

int incrementalfindfirsttet(struct behavior *behave,
                            struct inputs *in,
                            tag *tags,
                            struct proxipool *pool,
                            tag firsttet[4])
{
  starreal *point0, *point1, *point2, *point3;
  starreal ori;
  arraypoolulong i;

  firsttet[0] = tags[0];
  firsttet[1] = GHOSTVERTEX;
  firsttet[2] = GHOSTVERTEX;
  firsttet[3] = GHOSTVERTEX;

  if (in->vertexcount < 4) {
    /* There are not four affinely independent vertices. */
    return 1;
  }

  /* Make sure the second vertex is not coincident with the first vertex. */
  point0 = ((struct vertex *) proxipooltag2object(pool, tags[0]))->coord;
  point1 = ((struct vertex *) proxipooltag2object(pool, tags[1]))->coord;
  i = 1;
  while ((point0[0] == point1[0]) && (point0[1] == point1[1]) &&
         (point0[2] == point1[2])) {
    if (i >= in->vertexcount - 3) {
      /* There are not four affinely independent vertices. */
      return 1;
    }
    i++;
    point1 = ((struct vertex *) proxipooltag2object(pool, tags[i]))->coord;
  }
  firsttet[1] = tags[i];

  /* Make sure the third vertex is not collinear with the first two. */
  do {
    if (i >= in->vertexcount - 2) {
      /* There are not four affinely independent vertices. */
      return 1;
    }
    i++;
    point2 = ((struct vertex *) proxipooltag2object(pool, tags[i]))->coord;
  } while ((orient2d(behave, point0[0], point0[1], point1[0], point1[1],
                     point2[0], point2[1]) == 0.0) &&
           (orient2d(behave, point0[1], point0[2], point1[1], point1[2],
                     point2[1], point2[2]) == 0.0) &&
           (orient2d(behave, point0[2], point0[0], point1[2], point1[0],
                     point2[2], point2[0]) == 0.0));
  firsttet[2] = tags[i];

  do {
    i++;
    point3 = ((struct vertex *) proxipooltag2object(pool, tags[i]))->coord;
    ori = orient3d(behave, point0, point1, point2, point3);
    if (ori != 0.0) {
      if (ori > 0.0) {
        firsttet[3] = tags[i];
      } else {
        firsttet[3] = firsttet[2];
        firsttet[2] = tags[i];
      }
      return 0;
    }
  } while (i < in->vertexcount - 1);

  return 1;
}

void incrementalcheckdelaunay(struct behavior *behave,
                              struct tetcomplex *plex) {
  struct tetcomplexposition pos;
  tag nexttet[4];
  tag adjacencies[2];
  arraypoolulong horrors;

  printf("Checking Delaunay property of tetrahedral complex...\n");

  horrors = 0;
  tetcomplexiteratorinit(plex, &pos);
  tetcomplexiteratenoghosts(&pos, nexttet);
  while (nexttet[0] != STOP) {
    if (tetcomplexadjacencies(plex, nexttet[0], nexttet[1], nexttet[2],
                              adjacencies)) {
      if ((adjacencies[0] < adjacencies[1]) &&
          (adjacencies[1] != GHOSTVERTEX)) {
        horrors += vertexcheckdelaunay(behave, plex, nexttet[0], nexttet[1],
                                       nexttet[2], adjacencies);
      }
    }
    tetcomplexiteratenoghosts(&pos, nexttet);
  }

  if (horrors == 0) {
    printf("  With the omniscience of a fabled oracle, I declare the mesh ");
    if (behave->weighted == 0) {
      printf("Delaunay.\n");
    } else if (behave->weighted == 1) {
      printf("\n    weighted Delaunay.\n");
    } else {
      printf("regular.\n");
    }
  } else if (horrors == 1) {
    printf("  !! !! !! !! Exactly one disturbing perversion identified.\n");
  } else {
    printf("  !! !! !! !! %lu outrages spotted with terror.\n",
           (unsigned long) horrors);
  }
}

/*  behave:  Command line switches, operation counts, etc.                   */

void incrementaldelaunay(struct behavior *behave,
                         struct inputs *in,
                         struct proxipool *vertexpool,
                         struct outputs *out,
                         struct tetcomplex *plex)
{
  struct arraypool workarray;
  struct vertex *insertvertex;
  tag currenttriangle[3];
  tag foundtet[4];
  tag tet1st[4];
  tag *tags;
  tag inserttag;
  enum locateresult result;
  arraypoolulong vertexcount;
  arraypoolulong i;
  int degenerate;

  if (!behave->quiet) {
    printf("Constructing Delaunay triangulation by incremental method.\n");
  }

  if (behave->fullrandom) {
    if (behave->verbose) {
      printf("  Scrambling vertices.\n");
    }
    /* Randomize vertex order. */
  } else if (!behave->norandom) {
    if (behave->verbose) {
      printf("  Putting vertices into biased randomized insertion order.\n");
    }
    incrementalbrio(in);
  }

  tags = in->vertextags;
  degenerate = incrementalfindfirsttet(behave, in, tags, vertexpool, tet1st);
  if (degenerate) {
    printf("Error:  The input vertices are all coplanar.\n");
    starexit(1);
  }

  if (behave->verbose > 3) {
    printf("        Map of tags to vertex numbers (in insertion order):\n");
    for (i = 0; i < in->vertexcount; i++) {
      printf("          tag %lu = vertex # %lu\n",
             (unsigned long) tags[i],
             (unsigned long)
             ((struct vertex *) proxipooltag2object(vertexpool, tags[i]))
             ->number);
    }
  }

  if (behave->verbose) {
    printf("  Creating first tetrahedron.\n");
    if (behave->verbose > 1) {
      printf("    (Tags %lu, %lu, %lu, %lu).\n",
             (unsigned long) tags[0], (unsigned long) tags[1],
             (unsigned long) tags[2], (unsigned long) tags[3]);
      printf("    (Vertex nos. %lu, %lu, %lu, %lu).\n",
             (unsigned long)
             ((struct vertex *) proxipooltag2object(vertexpool, tags[0]))
             ->number,
             (unsigned long)
             ((struct vertex *) proxipooltag2object(vertexpool, tags[1]))
             ->number,
             (unsigned long)
             ((struct vertex *) proxipooltag2object(vertexpool, tags[2]))
             ->number,
             (unsigned long)
             ((struct vertex *) proxipooltag2object(vertexpool, tags[3]))
             ->number);
    }
  }

  tetcomplexinit(plex, vertexpool, behave->verbose);
#ifdef SELF_CHECK
  vertexcheckorientation(behave, plex,
                         tet1st[0], tet1st[1], tet1st[2], tet1st[3]);
#endif /* SELF_CHECK */
  tetcomplexinserttet(plex, tet1st[0], tet1st[1], tet1st[2], tet1st[3]);
  tetcomplexinserttet(plex, tet1st[0], tet1st[2], tet1st[1], GHOSTVERTEX);
  tetcomplexinserttet(plex, tet1st[0], tet1st[1], tet1st[3], GHOSTVERTEX);
  tetcomplexinserttet(plex, tet1st[0], tet1st[3], tet1st[2], GHOSTVERTEX);
  tetcomplexinserttet(plex, tet1st[3], tet1st[1], tet1st[2], GHOSTVERTEX);
  currenttriangle[0] = tet1st[0];
  currenttriangle[1] = tet1st[1];
  currenttriangle[2] = tet1st[2];

  if (behave->verbose) {
    printf("  Incrementally inserting vertices.\n");
  }

  vertexcount = 4;
  bowyerwatsonworkarrayinit(&workarray);
  for (i = 0; i < in->vertexcount; i++) {
    inserttag = tags[i];
    insertvertex = (struct vertex *) proxipooltag2object(vertexpool,
                                                         inserttag);
    if ((insertvertex->number != DEADVERTEX) &&
        (inserttag != tet1st[0]) && (inserttag != tet1st[1]) &&
        (inserttag != tet1st[2]) && (inserttag != tet1st[3])) {
      if (behave->verbose > 1) {
        printf("    Inserting vertex # %lu (tag %lu):\n",
               (unsigned long) insertvertex->number,
               (unsigned long) inserttag);
        printf("      (%.12g, %.12g, %.12g).\n",
               (double) insertvertex->coord[0],
               (double) insertvertex->coord[1],
               (double) insertvertex->coord[2]);
      }
      result = locatepoint(behave, plex, inserttag, currenttriangle[0],
                           currenttriangle[1], currenttriangle[2], foundtet);

      if (result == ONVERTEX) {
        printf("Warning:  A duplicate vertex at (%.12g, %.12g, %.12g) was "
               "ignored.\n", (double) insertvertex->coord[0],
               (double) insertvertex->coord[1],
               (double) insertvertex->coord[2]);
        /* Point to original? */
      } else if (result == BADSTART) {
        printf("Internal error in incrementaldelaunay():\n");
        printf("  Point location begun at nonexistent triangle.\n");
        internalerror();
      } else {
        /* Fix the following for weighted Delaunay. */
        vertexcount++;
#ifdef SELF_CHECK
        if (foundtet[3] != GHOSTVERTEX) {
          vertexcheckpointintet(behave, plex, foundtet[0], foundtet[1],
                                foundtet[2], foundtet[3], inserttag);
        }
#endif /* SELF_CHECK */
        bowyerwatson(behave, plex, &workarray, inserttag, foundtet[0],
                     foundtet[1], foundtet[2], foundtet[3], currenttriangle);
#ifdef PARANOID
        incrementalcheckdelaunay(behave, plex);
#endif /* PARANOID */
      }
    }
  }

  out->vertexcount = vertexcount;
  out->tetcount = tetcomplextetcount(plex);
  out->boundaryfacecount = tetcomplexghosttetcount(plex);
  out->facecount = 2 * out->tetcount + (out->boundaryfacecount / 2);
  out->edgecount = out->vertexcount + out->facecount - out->tetcount - 1;

  arraypooldeinit(&workarray);

  tetcomplexremoveghosttets(plex);
}

/**                                                                         **/
/**                                                                         **/
/********* Incremental insertion Delaunay routines end here          *********/


