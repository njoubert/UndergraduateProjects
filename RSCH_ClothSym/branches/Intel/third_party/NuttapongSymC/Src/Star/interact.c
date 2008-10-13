/********* User interaction routines begin here                      *********/
/**                                                                         **/
/**                                                                         **/

/*****************************************************************************/
/*                                                                           */
/*  internalerror()   Ask the user to send me the defective product.  Exit.  */
/*                                                                           */
/*****************************************************************************/

void internalerror(void)
{
  printf("  Please report this bug to jrs@cs.berkeley.edu\n");
  printf("  Include the message above, your input data set, and the exact\n");
  printf("  command line you used to run Star.\n");
  starexit(1);
}

/*****************************************************************************/
/*                                                                           */
/*  syntax()   Print a list of command line switches.                        */
/*                                                                           */
/*****************************************************************************/

#ifndef STARLIBRARY

void syntax(void)
{
#ifdef CDT_ONLY
  printf("Star [-pAcwWjefvngBPNEIOXzo_CQVh] input_file\n");
#else /* not CDT_ONLY */
  printf("Star [-prq__a__uAcwWDjefvngBPNEIOXzo_YSkK__CQVh] input_file\n");
#endif /* not CDT_ONLY */

  printf("    -p  Tetrahedralizes a polyhedron or PLC (.poly file).\n");
#ifndef CDT_ONLY
  printf("    -r  Refines a previously generated mesh.\n");
  printf("    -q  Quality mesh generation.\n");
  printf("    -a  Applies a maximum tetrahedron volume constraint.\n");
  printf("    -u  Applies a user-defined tetrahedron constraint.\n");
#endif /* not CDT_ONLY */
  printf("    -A  Applies attributes to identify tetrahedra in certain "
         "regions.\n");
  printf("    -c  Encloses the convex hull with facets.\n");
  printf("    -w  Weighted Delaunay triangulation.\n");
  printf("    -W  Regular triangulation (lower convex hull of a height "
         "field).\n");
#ifndef CDT_ONLY
  printf("    -D  Conforming Delaunay:  attempts to make tetrahedra "
         "Delaunay.\n");
#endif /* not CDT_ONLY */
  printf("    -j  Renumber nodes and jettison unused vertices from output.\n");
  printf("    -e  Generates an edge list (.edge file).\n");
  printf("    -f  Generates a face list (.face file).\n");
  printf("    -v  Generates a Voronoi diagram.\n");
  printf("    -n  Generates a list of tetrahedron neighbors (.neigh file).\n");
  printf("    -g  Generates an .off file for Geomview.\n");
  printf("    -B  Suppresses output of boundary information.\n");
  printf("    -P  Suppresses output of .poly file.\n");
  printf("    -N  Suppresses output of .node file.\n");
  printf("    -E  Suppresses output of .ele file.\n");
  printf("    -I  Suppresses mesh iteration numbers.\n");
  printf("    -O  Ignores holes in .poly file.\n");
  printf("    -X  Suppresses use of exact arithmetic.\n");
  printf("    -z  Numbers all items starting from zero (rather than one).\n");
  printf("    -o2 Generates second-order subparametric elements.\n");
#ifndef CDT_ONLY
  printf("    -Y  Suppresses boundary facet splitting.\n");
  printf("    -S  Specifies maximum number of added Steiner points.\n");
#endif /* not CDT_ONLY */
  printf("    -k  Insert vertices in original order (no randomization).\n");
  printf("    -K  Insert vertices in fully random order (instead of BRIO).\n");
  printf("    -C  Check consistency of final mesh.\n");
  printf("    -Q  Quiet:  No terminal output except errors.\n");
  printf("    -V  Verbose:  Detailed information on what I'm doing.\n");
  printf("    -h  Help:  Detailed instructions for Star.\n");
  printf("PRE-RELEASE CODE:  DO NOT DISTRIBUTE!!!\n");
  printf("EXPECT THIS CODE TO BE BUGGY AND SLOW.\n");
  starexit(0);
}

#endif /* not STARLIBRARY */

/*****************************************************************************/
/*                                                                           */
/*  info()   Print out complete instructions.                                */
/*                                                                           */
/*****************************************************************************/

#ifndef STARLIBRARY

void info(void)
{
  printf("Star\n");
  printf(
"A Three-Dimensional Quality Mesh Generator and Delaunay Tetrahedralizer.\n");
  printf("Version 0.1\n\n");
  printf("Copyright 1995, 1996, 1997, 1998 Jonathan Richard Shewchuk\n");
  printf("2360 Woolsey #H / Berkeley, California  94705-1927\n");
  printf("Bugs/comments to jrs@cs.berkeley.edu\n");
  printf(
"Created as part of the Quake project (tools for earthquake simulation).\n");
  printf(
"Supported in part by NSF Award CMS-9318163 and an NSERC 1967 Scholarship.\n");
  printf("There is no warranty whatsoever.  Use at your own risk.\n");
  printf("PRE-RELEASE CODE:  DO NOT DISTRIBUTE!!!\n");
  printf("EXPECT THIS CODE TO BE BUGGY.\n");
#ifdef SINGLE
  printf("This executable is compiled for single precision arithmetic.\n\n\n");
#else /* not SINGLE */
  printf("This executable is compiled for double precision arithmetic.\n\n\n");
#endif /* not SINGLE */
  starexit(0);
}

#endif /* not STARLIBRARY */

/*****************************************************************************/
/*                                                                           */
/*  parsecommandline()   Read the command line, identify switches, and set   */
/*                       up options and file names.                          */
/*                                                                           */
/*  argc:  The number of strings on the command line.                        */
/*  argv:  An array of the strings on the command line.                      */
/*  b:  The struct storing Star's options and file names.                    */
/*                                                                           */
/*****************************************************************************/

void parsecommandline(int argc, char **argv, struct behavior *b)
{
#ifdef STARLIBRARY
#define STARTINDEX 0
#else /* not STARLIBRARY */
#define STARTINDEX 1
  int increment;
  int meshnumber;
#endif /* not STARLIBRARY */
  int i, j, k;
  char workstring[FILENAMESIZE];

  /* See the comments above the definition of `struct behavior' for the */
  /*   meaning of most of these variables.                              */
  b->inspherecount = 0;
  b->orientcount = 0;
  b->orient4dcount = 0;
  b->tetcircumcentercount = 0;
  b->tricircumcentercount = 0;

  b->poly = b->refine = b->quality = 0;
  b->varvolume = b->fixedvolume = b->usertest = 0;
  b->regionattrib = b->convex = b->weighted = b->jettison = 0;
  b->firstnumber = 1;
  b->edgesout = b->facesout = b->voronoi = b->neighbors = b->geomview = 0;
  b->nobound = b->nopolywritten = b->nonodewritten = b->noelewritten = 0;
  b->noiterationnum = 0;
  b->noholes = b->noexact = 0;
  b->docheck = 0;
  b->nobisect = 0;
  b->conformdel = 0;
  /* A negative `steiner' value indicates no constraint on number of */
  /*   Steiner points.                                               */
  b->steiner = -1;
  b->order = 1;
  b->qmeasure = 0.0;
  b->minangle = 0.0;
  /* A negative volume indicates no constraint on tetrahedron volumes. */
  b->maxvolume = -1.0;
  b->quiet = b->verbose = 0;
#ifndef STARLIBRARY
  b->innodefilename[0] = '\0';
#endif /* not STARLIBRARY */

  for (i = STARTINDEX; i < argc; i++) {
#ifndef STARLIBRARY
    if (argv[i][0] == '-') {
#endif /* not STARLIBRARY */
      for (j = STARTINDEX; argv[i][j] != '\0'; j++) {
        if (argv[i][j] == 'p') {
          b->poly = 1;
	}
#ifndef CDT_ONLY
        if (argv[i][j] == 'r') {
          b->refine = 1;
	}
        if (argv[i][j] == 'q') {
          b->quality = 1;
          if (((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) ||
              (argv[i][j + 1] == '.')) {
            /* Read a numerical bound on the largest permissible */
            /*   circumradius-to-shortest edge ratio.            */
            k = 0;
            while (((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) ||
                   (argv[i][j + 1] == '.')) {
              j++;
              workstring[k] = argv[i][j];
              k++;
            }
            workstring[k] = '\0';
            b->qmeasure = (starreal) strtod(workstring, (char **) NULL);
          } else {
            /* Largest permissible circumradius-to-shortest edge ratio is 2. */
            b->qmeasure = 2.0;
          }
          if ((argv[i][j + 1] == '/') || (argv[i][j + 1] == ',')) {
            j++;
            if (((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) ||
                (argv[i][j + 1] == '.')) {
              /* Read a numerical bound on the smallest permissible */
              /*   dihedral angle.                                  */
              k = 0;
              while (((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) ||
                     (argv[i][j + 1] == '.')) {
                j++;
                workstring[k] = argv[i][j];
                k++;
              }
              workstring[k] = '\0';
              b->minangle = (starreal) strtod(workstring, (char **) NULL);
            } else {
              /* Smallest permissible dihedral angle is 5 degrees. */
              b->minangle = 5.0;
            }
          }
        }
        if (argv[i][j] == 'a') {
          b->quality = 1;
          if (((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) ||
              (argv[i][j + 1] == '.')) {
            b->fixedvolume = 1;
            /* Read a numerical bound on the largest permissible */
            /*   tetrahedron volume.                             */
            k = 0;
            while (((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) ||
                   (argv[i][j + 1] == '.')) {
              j++;
              workstring[k] = argv[i][j];
              k++;
            }
            workstring[k] = '\0';
            b->maxvolume = (starreal) strtod(workstring, (char **) NULL);
            if (b->maxvolume <= 0.0) {
              printf("Error:  Maximum volume must be greater than zero.\n");
              starexit(1);
	    }
	  } else {
            b->varvolume = 1;
	  }
	}
        if (argv[i][j] == 'u') {
          b->quality = 1;
          b->usertest = 1;
        }
#endif /* not CDT_ONLY */
        if (argv[i][j] == 'A') {
          b->regionattrib = 1;
        }
        if (argv[i][j] == 'c') {
          b->convex = 1;
        }
        if (argv[i][j] == 'w') {
          b->weighted = 1;
        }
        if (argv[i][j] == 'W') {
          b->weighted = 2;
        }
        if (argv[i][j] == 'j') {
          b->jettison = 1;
        }
        if (argv[i][j] == 'z') {
          b->firstnumber = 0;
        }
        if (argv[i][j] == 'e') {
          b->edgesout = 1;
	}
        if (argv[i][j] == 'f') {
          b->facesout = 1;
	}
        if (argv[i][j] == 'v') {
          b->voronoi = 1;
	}
        if (argv[i][j] == 'n') {
          b->neighbors = 1;
	}
        if (argv[i][j] == 'g') {
          b->geomview = 1;
	}
        if (argv[i][j] == 'B') {
          b->nobound = 1;
	}
        if (argv[i][j] == 'P') {
          b->nopolywritten = 1;
	}
        if (argv[i][j] == 'N') {
          b->nonodewritten = 1;
	}
        if (argv[i][j] == 'E') {
          b->noelewritten = 1;
	}
#ifndef STARLIBRARY
        if (argv[i][j] == 'I') {
          b->noiterationnum = 1;
	}
#endif /* not STARLIBRARY */
        if (argv[i][j] == 'O') {
          b->noholes = 1;
	}
        if (argv[i][j] == 'X') {
          b->noexact = 1;
	}
        if (argv[i][j] == 'o') {
          if (argv[i][j + 1] == '2') {
            j++;
            b->order = 2;
          }
	}
#ifndef CDT_ONLY
        if (argv[i][j] == 'Y') {
          b->nobisect++;
	}
        if (argv[i][j] == 'S') {
          b->steiner = 0;
          /* Read a numerical bound on the maximum number of Steiner points. */
          while ((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) {
            j++;
            b->steiner = b->steiner * 10l + (starlong) (argv[i][j] - '0');
          }
        }
        if (argv[i][j] == 'D') {
          b->conformdel = 1;
        }
#endif /* not CDT_ONLY */
        if (argv[i][j] == 'k') {
          b->norandom = 1;
        }
        if (argv[i][j] == 'K') {
          b->fullrandom = 1;
        }
        if (argv[i][j] == 'C') {
          b->docheck = 1;
        }
        if (argv[i][j] == 'Q') {
          b->quiet = 1;
        }
        if (argv[i][j] == 'V') {
          b->verbose++;
        }
#ifndef STARLIBRARY
        if ((argv[i][j] == 'h') || (argv[i][j] == 'H') ||
            (argv[i][j] == '?')) {
          info();
	}
#endif /* not STARLIBRARY */
      }
#ifndef STARLIBRARY
    } else {
      /* Any command-line parameter not starting with "-" is assumed to be */
      /*   a file name.                                                    */
      strncpy(b->innodefilename, argv[i], FILENAMESIZE - 1);
      b->innodefilename[FILENAMESIZE - 1] = '\0';
    }
#endif /* not STARLIBRARY */
  }

#ifndef STARLIBRARY
  if (b->innodefilename[0] == '\0') {
    /* No file name specified; print a summary of the command line switches. */
    syntax();
  }

  /* Remove the filename extension (if any) and use it to infer switches */
  /*   that might not have been specified directly.                      */
  if (!strcmp(&b->innodefilename[strlen(b->innodefilename) - 5], ".node")) {
    b->innodefilename[strlen(b->innodefilename) - 5] = '\0';
  }
  if (!strcmp(&b->innodefilename[strlen(b->innodefilename) - 5], ".poly")) {
    b->innodefilename[strlen(b->innodefilename) - 5] = '\0';
    b->poly = 1;
  }
#ifndef CDT_ONLY
  if (!strcmp(&b->innodefilename[strlen(b->innodefilename) - 4], ".ele")) {
    b->innodefilename[strlen(b->innodefilename) - 4] = '\0';
    b->refine = 1;
  }
  if (!strcmp(&b->innodefilename[strlen(b->innodefilename) - 5], ".area")) {
    b->innodefilename[strlen(b->innodefilename) - 5] = '\0';
    b->refine = 1;
    b->quality = 1;
    b->varvolume = 1;
  }
#endif /* not CDT_ONLY */
#endif /* not STARLIBRARY */

  b->addvertices = b->poly || b->quality;
  b->usefacets = b->poly || b->refine || b->quality || b->convex;
  /* Compute the square of the cosine of the dihedral angle bound. */
  b->goodangle = cos(b->minangle * ST_PI / 180.0);
  b->goodangle *= b->goodangle;
  if (b->refine && b->noiterationnum) {
    printf(
      "Error:  You cannot use the -I switch when refining a triangulation.\n");
    starexit(1);
  }
  /* Be careful not to allocate space for element volume constraints that */
  /*   will never be assigned any value (other than the default -1.0).    */
  if (!b->refine && !b->poly) {
    b->varvolume = 0;
  }
  /* Be careful not to add an extra attribute to each element unless the */
  /*   input supports it (PLC in, but not refining a preexisting mesh).  */
  if (b->refine || !b->poly) {
    b->regionattrib = 0;
  }
  /* Regular/weighted triangulations are incompatible with PLCs and meshing. */
  if (b->weighted && (b->poly || b->quality)) {
    b->weighted = 0;
    if (!b->quiet) {
      printf("Warning:  weighted triangulations (-w, -W) are incompatible\n");
      printf("  with PLCs (-p) and meshing (-q, -a, -u).  Weights ignored.\n");
    }
  }
  if (b->jettison && b->noiterationnum && !b->poly && !b->quiet) {
    printf("Warning:  -j and -I switches are largely incompatible.\n");
    printf("  The vertices are renumbered, so you will need the output\n");
    printf("  .node file to determine the new node indices.");
  } else if (b->jettison && b->nonodewritten && !b->quiet) {
    printf("Warning:  -j and -N switches are somewhat incompatible.\n");
    printf("  The vertices are renumbered, so you will need the output\n");
    printf("  .node file to determine the new node indices.");
  }

#ifndef STARLIBRARY
  strcpy(b->inpolyfilename, b->innodefilename);
  strcpy(b->inelefilename, b->innodefilename);
  strcpy(b->areafilename, b->innodefilename);
  /* Check the input filename for an iteration number. */
  increment = 0;
  strcpy(workstring, b->innodefilename);
  /* Find the last period in the filename. */
  j = 1;
  while (workstring[j] != '\0') {
    if ((workstring[j] == '.') && (workstring[j + 1] != '\0')) {
      increment = j + 1;
    }
    j++;
  }
  /* The iteration number is zero by default, unless there's an iteration */
  /*   number in the filename.                                            */
  meshnumber = 0;
  if (increment > 0) {
    /* Read the iteration number from the end of the filename. */
    j = increment;
    do {
      if ((workstring[j] >= '0') && (workstring[j] <= '9')) {
        meshnumber = meshnumber * 10 + (int) (workstring[j] - '0');
      } else {
        /* Oops, not a digit; this isn't an iteration number after all. */
        increment = 0;
      }
      j++;
    } while (workstring[j] != '\0');
  }
  if (b->noiterationnum) {
    /* Do not use iteration numbers. */
    strcpy(b->outnodefilename, b->innodefilename);
    strcpy(b->outelefilename, b->innodefilename);
    strcpy(b->edgefilename, b->innodefilename);
    strcpy(b->facefilename, b->innodefilename);
    strcpy(b->vnodefilename, b->innodefilename);
    strcpy(b->vpolyfilename, b->innodefilename);
    strcpy(b->neighborfilename, b->innodefilename);
    strcpy(b->offfilename, b->innodefilename);
    strcat(b->outnodefilename, ".node");
    strcat(b->outelefilename, ".ele");
    strcat(b->edgefilename, ".edge");
    strcat(b->facefilename, ".face");
    strcat(b->vnodefilename, ".v.node");
    strcat(b->vpolyfilename, ".v.poly");
    strcat(b->neighborfilename, ".neigh");
    strcat(b->offfilename, ".off");
  } else if (increment == 0) {
    /* Input iteration number is zero; output iteration number is one. */
    strcpy(b->outnodefilename, b->innodefilename);
    strcpy(b->outpolyfilename, b->innodefilename);
    strcpy(b->outelefilename, b->innodefilename);
    strcpy(b->edgefilename, b->innodefilename);
    strcpy(b->facefilename, b->innodefilename);
    strcpy(b->vnodefilename, b->innodefilename);
    strcpy(b->vpolyfilename, b->innodefilename);
    strcpy(b->neighborfilename, b->innodefilename);
    strcpy(b->offfilename, b->innodefilename);
    strcat(b->outnodefilename, ".1.node");
    strcat(b->outpolyfilename, ".1.poly");
    strcat(b->outelefilename, ".1.ele");
    strcat(b->edgefilename, ".1.edge");
    strcat(b->facefilename, ".1.face");
    strcat(b->vnodefilename, ".1.v.node");
    strcat(b->vpolyfilename, ".1.v.poly");
    strcat(b->neighborfilename, ".1.neigh");
    strcat(b->offfilename, ".1.off");
  } else {
    /* Higher iteration numbers. */
    workstring[increment] = '%';
    workstring[increment + 1] = 'd';
    workstring[increment + 2] = '\0';
    sprintf(b->outnodefilename, workstring, meshnumber + 1);
    strcpy(b->outpolyfilename, b->outnodefilename);
    strcpy(b->outelefilename, b->outnodefilename);
    strcpy(b->edgefilename, b->outnodefilename);
    strcpy(b->facefilename, b->outnodefilename);
    strcpy(b->vnodefilename, b->outnodefilename);
    strcpy(b->vpolyfilename, b->outnodefilename);
    strcpy(b->neighborfilename, b->outnodefilename);
    strcpy(b->offfilename, b->outnodefilename);
    strcat(b->outnodefilename, ".node");
    strcat(b->outpolyfilename, ".poly");
    strcat(b->outelefilename, ".ele");
    strcat(b->edgefilename, ".edge");
    strcat(b->facefilename, ".face");
    strcat(b->vnodefilename, ".v.node");
    strcat(b->vpolyfilename, ".v.poly");
    strcat(b->neighborfilename, ".neigh");
    strcat(b->offfilename, ".off");
  }
  strcat(b->innodefilename, ".node");
  strcat(b->inpolyfilename, ".poly");
  strcat(b->inelefilename, ".ele");
  strcat(b->areafilename, ".area");
#endif /* not STARLIBRARY */
}

/**                                                                         **/
/**                                                                         **/
/********* User interaction routines end here                        *********/


