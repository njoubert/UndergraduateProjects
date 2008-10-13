#ifndef NOMAIN
/*****************************************************************************/
/*                                                                           */
/*  main()   Gosh, do everything.                                            */
/*                                                                           */
/*  The sequence is roughly as follows.  Many of these steps can be skipped, */
/*  depending on the command line switches.                                  */
/*                                                                           */
/*  - Initialize constants and parse the command line.                       */
/*  - Read the vertices from a file and either                               */
/*    - tetrahedralize them (no -r), or                                      */
/*    - read an old mesh from files and reconstruct it (-r).                 */
/*  - Insert the segments and facets (-p).  This step may also reinforce the */
/*      faces on the boundary of the convex hull against concavity carving   */
/*      (-c).                                                                */
/*  - Read the holes (-p), regional attributes (-pA), and regional volume    */
/*      constraints (-pa).  Carve the holes and concavities, and spread the  */
/*      regional attributes and volume constraints.                          */
/*  - Enforce the constraints on tetrahedron quality (-q) and maximum volume */
/*      (-a).  Also enforce the conforming constrained Delaunay property     */
/*      (-q and/or -a).                                                      */
/*  - Compute the number of edges and faces in the resulting mesh.           */
/*  - Promote the mesh's linear tetrahedra to higher order elements (-o).    */
/*  - Write the output files and print the statistics.                       */
/*  - Check the consistency and Delaunay property of the mesh (-C).          */
/*                                                                           */
/*****************************************************************************/

int main(int argc,
         char **argv)
{
  struct behavior behave;
  struct inputs in;
  struct outputs out;
  struct proxipool vertexpool;
  struct tetcomplex mesh;
#ifndef NO_TIMER
  /* Variables for timing the performance of Star.  The types are */
  /*   defined in sys/time.h.                                     */
  struct timeval tv0, tv1, tv2, tv3, tv4, tv5, tv6;
  struct timezone tz;
#endif /* not NO_TIMER */
  struct segment *segmentlist;
  FILE *polyfile;

#ifndef NO_TIMER
  gettimeofday(&tv0, &tz);
#endif /* not NO_TIMER */

  primitivesinit();
  parsecommandline(argc, argv, &behave);
  polyfile = inputverticesreadsortstore(&behave, &in, &vertexpool);

#ifndef NO_TIMER
  if (!behave.quiet) {
    gettimeofday(&tv1, &tz);
  }
#endif /* not NO_TIMER */

#ifdef CDT_ONLY
  /* Tetrahedralize the vertices. */
  incrementaldelaunay(&behave, &in, &vertexpool, &out, &mesh);
  /* Compute an array mapping vertex numbers to their tags. */
  inputmaketagmap(&vertexpool, in.firstnumber, in.vertextags);
#else /* not CDT_ONLY */
  if (behave.refine) {
    /* Compute an array mapping vertex numbers to their tags. */
    inputmaketagmap(&vertexpool, in.firstnumber, in.vertextags);
    /* Read and reconstruct a mesh. */
    inputtetrahedra(&behave, &in, &vertexpool, &out, &mesh);
  } else {
    /* Tetrahedralize the vertices. */
    incrementaldelaunay(&behave, &in, &vertexpool, &out, &mesh);
    /* Compute an array mapping vertex numbers to their tags. */
    inputmaketagmap(&vertexpool, in.firstnumber, in.vertextags);
  }
#endif /* not CDT_ONLY */

#ifndef NO_TIMER
  if (!behave.quiet) {
    gettimeofday(&tv2, &tz);
    if (behave.refine) {
      printf("Mesh reconstruction");
    } else {
      printf("Delaunay");
    }
    printf(" milliseconds:  %ld\n", 1000l * (tv2.tv_sec - tv1.tv_sec) +
           (tv2.tv_usec - tv1.tv_usec) / 1000l);
  }
#endif /* not NO_TIMER */

  if (polyfile != (FILE *) NULL) {
    if (!behave.quiet) {
      printf("Recovering segments and facets in "
             "Delaunay tetrahedralization.\n");
    }
    segmentlist = constrainreadrecoversegments(&behave, &in, polyfile,
                                               &vertexpool, &out, &mesh);
    fclose(polyfile);
  }

#ifndef NO_TIMER
  if (!behave.quiet) {
    gettimeofday(&tv3, &tz);
    if (behave.usefacets && !behave.refine) {
      printf("Subsegment/subfacet milliseconds:  %ld\n",
             1000l * (tv3.tv_sec - tv2.tv_sec) +
             (tv3.tv_usec - tv2.tv_usec) / 1000l);
    }
  }
#endif /* not NO_TIMER */

#ifndef NO_TIMER
  if (!behave.quiet) {
    gettimeofday(&tv4, &tz);
    if (behave.poly && !behave.refine) {
      printf("Hole milliseconds:  %ld\n", 1000l * (tv4.tv_sec - tv3.tv_sec) +
             (tv4.tv_usec - tv3.tv_usec) / 1000l);
    }
  }
#endif /* not NO_TIMER */

#ifndef NO_TIMER
  if (!behave.quiet) {
    gettimeofday(&tv5, &tz);
#ifndef CDT_ONLY
    if (behave.quality) {
      printf("Quality milliseconds:  %ld\n",
             1000l * (tv5.tv_sec - tv4.tv_sec) +
             (tv5.tv_usec - tv4.tv_usec) / 1000l);
    }
#endif /* not CDT_ONLY */
  }
#endif /* not NO_TIMER */

  if (behave.order > 1) {
    /* Promote elements to higher polynomial order. */
/*    highorder(); */
  }
  if (!behave.quiet) {
    printf("\n");
  }

  outputpreparevertices(&behave, &mesh);
  /* If not using iteration numbers, don't write a .node file if one was */
  /*   read, because the original one would be overwritten!              */
  if (behave.nonodewritten ||
      (behave.noiterationnum && behave.readnodefileflag)) {
    if (!behave.quiet) {
      printf("NOT writing a .node file.\n");
    }
    /* We must remember to number the vertices. */
    outputnumbervertices(&behave, &in, &vertexpool);
  } else {
    /* Numbers the vertices too. */
    outputvertices(&behave, &in, &vertexpool, out.vertexcount, argc, argv);
  }

  if (behave.noelewritten) {
    if (!behave.quiet) {
      printf("NOT writing an .ele file.\n");
    }
  } else {
    outputtetrahedra(&behave, &in, &mesh, argc, argv);
  }
  if (behave.edgesout) {
    outputedges(&behave, &in, &mesh, out.edgecount, argc, argv);
  }
  if (behave.facesout) {
    outputfaces(&behave, &in, &mesh, out.facecount, argc, argv);
  }

  if (!behave.quiet) {
#ifndef NO_TIMER
    gettimeofday(&tv6, &tz);
    printf("\nOutput milliseconds:  %ld\n",
           1000l * (tv6.tv_sec - tv5.tv_sec) +
           (tv6.tv_usec - tv5.tv_usec) / 1000l);
    printf("Total running milliseconds:  %ld\n",
           1000l * (tv6.tv_sec - tv0.tv_sec) +
           (tv6.tv_usec - tv0.tv_usec) / 1000l);
#endif /* not NO_TIMER */

    statistics(&behave, &in, &out, &mesh);
  }

  if (behave.docheck) {
    tetcomplexconsistency(&mesh);
    incrementalcheckdelaunay(&behave, &mesh);
  }

  /* Free the mesh initialized by incrementaldelaunay() or */
  /*   inputtetrahedra().                                  */
  tetcomplexdeinit(&mesh);
  /* Free the array of vertex tags allocated by inputall(). */
  if (in.vertextags != (tag *) NULL) {
    starfree(in.vertextags);
    in.vertextags = (tag *) NULL;
  }
  /* Free the pool of vertices initialized by inputall(). */
  proxipooldeinit(&vertexpool);

  return 0;
}
#endif /* NOMAIN */
