/********* Statistics printing routines begin here                   *********/
/**                                                                         **/
/**                                                                         **/

void statisticsquality(struct behavior *behave,
                       struct inputs *in,
                       struct tetcomplex *plex)
{
#define BINS 90                                              // Must be even!!!

  struct tetcomplexposition position;
  tag tet[4];
  starreal *point[4];
  starreal tcenter[3];
  double tansquaretable[8];
  double tansquaretable2[BINS / 2 - 1];
  double aspecttable[16];
  double circumtable[16];
  double edgelength[3][4];
  double facenormal[4][3];
  double dx, dy, dz;
  double dotproduct;
  double tansquare;
  double pyrvolume;
  double facearea2;
  double pyrbiggestface2;
  double shortest, longest;
  double pyrshortest2, pyrlongest2;
  double smallestvolume, biggestvolume;
  double pyrminaltitude2;
  double minaltitude;
  double pyraspect2;
  double worstaspect;
  double pyrcircumratio2;
  double worstcircumratio;
  double smallestangle, biggestangle;
  double radconst, degconst;
  arraypoolulong anglecount[18];
  arraypoolulong anglecount2[BINS];
  arraypoolulong aspectcount[16];
  arraypoolulong circumcount[16];
  int aspectindex;
  int circumindex;
  int tendegree;
  int bindegree;
  int acutebiggestflag;
  int firstiterationflag;
  int i, ii, j, k, l;

  printf("Mesh quality statistics:\n\n");
  radconst = ST_PI / 18.0;
  degconst = 180.0 / ST_PI;
  for (i = 0; i < 8; i++) {
    tansquaretable[i] = tan(radconst * (double) (i + 1));
    tansquaretable[i] = tansquaretable[i] * tansquaretable[i];
  }
  radconst = ST_PI / (double) BINS;
  for (i = 0; i < BINS / 2 - 1; i++) {
    tansquaretable2[i] = tan(radconst * (double) (i + 1));
    tansquaretable2[i] = tansquaretable2[i] * tansquaretable2[i] - 0.00000001;
  }
  for (i = 0; i < 18; i++) {
    anglecount[i] = 0;
  }
  for (i = 0; i < BINS; i++) {
    anglecount2[i] = 0;
  }

  aspecttable[0]  =      1.5;      aspecttable[1]  =     2.0;
  aspecttable[2]  =      2.5;      aspecttable[3]  =     3.0;
  aspecttable[4]  =      4.0;      aspecttable[5]  =     6.0;
  aspecttable[6]  =     10.0;      aspecttable[7]  =    15.0;
  aspecttable[8]  =     25.0;      aspecttable[9]  =    50.0;
  aspecttable[10] =    100.0;      aspecttable[11] =   300.0;
  aspecttable[12] =   1000.0;      aspecttable[13] = 10000.0;
  aspecttable[14] = 100000.0;      aspecttable[15] =     0.0;
  for (i = 0; i < 16; i++) {
    aspectcount[i] = 0;
  }

  circumtable[0]  =      0.75;     circumtable[1]  =     1.0;
  circumtable[2]  =      1.5;      circumtable[3]  =     2.0;
  circumtable[4]  =      3.0;      circumtable[5]  =     5.0;
  circumtable[6]  =     10.0;      circumtable[7]  =    15.0;
  circumtable[8]  =     25.0;      circumtable[9]  =    50.0;
  circumtable[10] =    100.0;      circumtable[11] =   300.0;
  circumtable[12] =   1000.0;      circumtable[13] = 10000.0;
  circumtable[14] = 100000.0;      circumtable[15] =     0.0;
  for (i = 0; i < 16; i++) {
    circumcount[i] = 0;
  }

  shortest = 0.0;
  longest = 0.0;
  smallestvolume = 0.0;
  biggestvolume = 0.0;
  minaltitude = 0.0;
  worstaspect = 0.0;
  worstcircumratio = 0.0;
  smallestangle = 100.0;
  biggestangle = 0.0;
  acutebiggestflag = 1;

  firstiterationflag = 1;
  tetcomplexiteratorinit(plex, &position);
  tetcomplexiteratenoghosts(&position, tet);
  while (tet[0] != STOP) {
    point[0] = ((struct vertex *) tetcomplextag2vertex(plex, tet[0]))->coord;
    point[1] = ((struct vertex *) tetcomplextag2vertex(plex, tet[1]))->coord;
    point[2] = ((struct vertex *) tetcomplextag2vertex(plex, tet[2]))->coord;
    point[3] = ((struct vertex *) tetcomplextag2vertex(plex, tet[3]))->coord;
    pyrshortest2 = 0.0;
    pyrlongest2 = 0.0;
    pyrbiggestface2 = 0.0;

    for (i = 0; i < 4; i++) {
      j = (i + 1) & 3;
      if ((i & 1) == 0) {
        k = (i + 3) & 3;
        l = (i + 2) & 3;
      } else {
        k = (i + 2) & 3;
        l = (i + 3) & 3;
      }

      facenormal[i][0] =
        (point[k][1] - point[j][1]) * (point[l][2] - point[j][2]) -
        (point[k][2] - point[j][2]) * (point[l][1] - point[j][1]);
      facenormal[i][1] =
        (point[k][2] - point[j][2]) * (point[l][0] - point[j][0]) -
        (point[k][0] - point[j][0]) * (point[l][2] - point[j][2]);
      facenormal[i][2] =
        (point[k][0] - point[j][0]) * (point[l][1] - point[j][1]) -
        (point[k][1] - point[j][1]) * (point[l][0] - point[j][0]);
      facearea2 = facenormal[i][0] * facenormal[i][0] +
                  facenormal[i][1] * facenormal[i][1] +
                  facenormal[i][2] * facenormal[i][2];
      if (facearea2 > pyrbiggestface2) {
        pyrbiggestface2 = facearea2;
      }

      for (j = i + 1; j < 4; j++) {
        dx = point[i][0] - point[j][0];
        dy = point[i][1] - point[j][1];
        dz = point[i][2] - point[j][2];
        edgelength[i][j] = dx * dx + dy * dy + dz * dz;
        if (edgelength[i][j] > pyrlongest2) {
          pyrlongest2 = edgelength[i][j];
        }
        if ((j == 1) || (edgelength[i][j] < pyrshortest2)) {
          pyrshortest2 = edgelength[i][j];
        }
      }
    }
    if (pyrlongest2 > longest) {
      longest = pyrlongest2;
    }
    if ((pyrshortest2 < shortest) || firstiterationflag) {
      shortest = pyrshortest2;
    }

    pyrvolume = (double)
      orient3d(behave, point[0], point[1], point[2], point[3]);
    if ((pyrvolume < smallestvolume) || firstiterationflag) {
      smallestvolume = pyrvolume;
    }
    if (pyrvolume > biggestvolume) {
      biggestvolume = pyrvolume;
    }
    pyrminaltitude2 = pyrvolume * pyrvolume / pyrbiggestface2;
    if ((pyrminaltitude2 < minaltitude) || firstiterationflag) {
      minaltitude = pyrminaltitude2;
    }
    pyraspect2 = pyrlongest2 / pyrminaltitude2;
    if (pyraspect2 > worstaspect) {
      worstaspect = pyraspect2;
    }
    aspectindex = 0;
    while ((pyraspect2 >
            aspecttable[aspectindex] * aspecttable[aspectindex]) &&
           (aspectindex < 15)) {
      aspectindex++;
    }
    aspectcount[aspectindex]++;

    tetcircumcenter(behave, point[0], point[1], point[2], point[3], tcenter,
                    (starreal *) NULL, (starreal *) NULL, (starreal *) NULL);
    pyrcircumratio2 = (tcenter[0] * tcenter[0] + tcenter[1] * tcenter[1] +
                       tcenter[2] * tcenter[2]) / pyrshortest2;
    if (pyrcircumratio2 > worstcircumratio) {
      worstcircumratio = pyrcircumratio2;
    }
    circumindex = 0;
    while ((pyrcircumratio2 >
            circumtable[circumindex] * circumtable[circumindex]) &&
           (circumindex < 15)) {
      circumindex++;
    }
    circumcount[circumindex]++;

    for (i = 0; i < 3; i++) {
      for (j = i + 1; j < 4; j++) {
        k = (i > 0) ? 0 : (j > 1) ? 1 : 2;
        l = 6 - i - j - k;
        dotproduct = facenormal[i][0] * facenormal[j][0] +
                     facenormal[i][1] * facenormal[j][1] +
                     facenormal[i][2] * facenormal[j][2];
        if (dotproduct == 0.0) {
          anglecount[9]++;
          anglecount2[BINS / 2]++;
          if (acutebiggestflag) {
            biggestangle = 1.0e+300;
            acutebiggestflag = 0;
          }
        } else {
          tansquare = pyrvolume * pyrvolume * edgelength[k][l] /
                      (dotproduct * dotproduct);
          tendegree = 8;
          bindegree = BINS / 2 - 1;
          for (ii = 7; ii >= 0; ii--) {
            if (tansquare < tansquaretable[ii]) {
              tendegree = ii;
            }
          }
          for (ii = BINS / 2 - 2; ii >= 0; ii--) {
            if (tansquare < tansquaretable2[ii]) {
              bindegree = ii;
            }
          }
          if (dotproduct < 0.0) {
            anglecount[tendegree]++;
            if (tansquare > 100000000.0) {
              bindegree = BINS / 2;
            }
            anglecount2[bindegree]++;
            if (tansquare < smallestangle) {
              smallestangle = tansquare;
            }
            if (acutebiggestflag && (tansquare > biggestangle)) {
              biggestangle = tansquare;
            }
          } else {
            anglecount[17 - tendegree]++;
            anglecount2[BINS - 1 - bindegree]++;
            if (acutebiggestflag || (tansquare < biggestangle)) {
              biggestangle = tansquare;
              acutebiggestflag = 0;
            }
          }
        }
      }
    }

    tetcomplexiteratenoghosts(&position, tet);
    firstiterationflag = 0;
  }

  shortest = sqrt(shortest);
  longest = sqrt(longest);
  minaltitude = sqrt(minaltitude);
  worstaspect = sqrt(worstaspect);
  worstcircumratio = sqrt(worstcircumratio);
  smallestvolume /= 6.0;
  biggestvolume /= 6.0;
  smallestangle = degconst * atan(sqrt(smallestangle));
  if (acutebiggestflag) {
    biggestangle = degconst * atan(sqrt(biggestangle));
  } else {
    biggestangle = 180.0 - degconst * atan(sqrt(biggestangle));
  }

  printf("  Smallest volume: %14.5g   |  Largest volume: %14.5g\n",
         smallestvolume, biggestvolume);
  printf("  Shortest edge: %16.5g   |  Longest edge: %16.5g\n",
         shortest, longest);
  printf("  Shortest altitude: %12.5g   |  Largest aspect ratio: %8.5g\n\n",
         minaltitude, worstaspect);

  printf("  Tetrahedron aspect ratio histogram:\n");
  printf("  1.2247 - %-6.6g    :   %8lu   | %6.6g - %-6.6g     :  %8lu\n",
         aspecttable[0], (unsigned long) aspectcount[0],
         aspecttable[7], aspecttable[8], (unsigned long) aspectcount[8]);
  for (i = 1; i < 7; i++) {
    printf("  %6.6g - %-6.6g    :   %8lu   | %6.6g - %-6.6g     :  %8lu\n",
           aspecttable[i - 1], aspecttable[i], (unsigned long) aspectcount[i],
           aspecttable[i + 7], aspecttable[i + 8],
           (unsigned long) aspectcount[i + 8]);
  }
  printf("  %6.6g - %-6.6g    :   %8lu   | %6.6g -            :  %8lu\n",
         aspecttable[6], aspecttable[7], (unsigned long) aspectcount[7],
         aspecttable[14], (unsigned long) aspectcount[15]);
  printf("  (Aspect ratio is longest edge divided by shortest altitude)\n");

  printf("\n  Largest circumradius-to-shortest-edge ratio: %22.5g\n\n",
         worstcircumratio);

  printf("  Tetrahedron circumradius-to-shortest-edge ratio histogram:\n");
  printf("  0.6123 - %-6.6g    :   %8lu   | %6.6g - %-6.6g     :  %8lu\n",
         circumtable[0], (unsigned long) circumcount[0],
         circumtable[7], circumtable[8], (unsigned long) circumcount[8]);
  for (i = 1; i < 7; i++) {
    printf("  %6.6g - %-6.6g    :   %8lu   | %6.6g - %-6.6g     :  %8lu\n",
           circumtable[i - 1], circumtable[i], (unsigned long) circumcount[i],
           circumtable[i + 7], circumtable[i + 8],
           (unsigned long) circumcount[i + 8]);
  }
  printf("  %6.6g - %-6.6g    :   %8lu   | %6.6g -            :  %8lu\n",
         circumtable[6], circumtable[7], (unsigned long) circumcount[7],
         circumtable[14], (unsigned long) circumcount[15]);

  printf("\n  Smallest dihedral: %12.5g   |  Largest dihedral: %12.5g\n\n",
         smallestangle, biggestangle);

  printf("  Dihedral angle histogram:\n");
  for (i = 0; i < 9; i++) {
    printf("      %2d - %2d degrees:   %8lu   |    %3d - %3d degrees:  %8lu\n",
           i * 10, i * 10 + 10, (unsigned long) anglecount[i],
           i * 10 + 90, i * 10 + 100, (unsigned long) anglecount[i + 9]);
  }
  printf("\n");

  printf("  For a histogram, input the following line to hist.c:\n  ");
  for (i = 0; i < BINS; i++) {
    printf("%lu ", anglecount2[i]);
  }
  printf("%12.7g %12.7g\n\n", smallestangle, biggestangle);
}

void statistics(struct behavior *behave,
                struct inputs *in,
                struct outputs *out,
                struct tetcomplex *plex)
{
  arraypoolulong outsubfacs = 0;
  arraypoolulong outsubsegs = 0;

  printf("\nStatistics:\n\n");
  printf("  Input vertices: %lu\n", (unsigned long) in->vertexcount);
  if (behave->refine) {
    printf("  Input tetrahedra: %lu\n", (unsigned long) in->tetcount);
  }
  if (behave->poly) {
    printf("  Input segments: %lu\n", (unsigned long) in->segmentcount);
    printf("  Input facets: %lu\n", (unsigned long) in->facetcount);
    if (!behave->refine) {
      printf("  Input holes: %lu\n", (unsigned long) in->holecount);
    }
  }

  printf("\n  Mesh vertices: %lu\n", (unsigned long) out->vertexcount);
  printf("  Mesh tetrahedra: %lu\n", (unsigned long) out->tetcount);
  printf("  Mesh triangular faces: %lu\n", (unsigned long) out->facecount);
  printf("  Mesh edges: %lu\n", (unsigned long) out->edgecount);
  printf("  Mesh exterior boundary faces: %lu\n",
         (unsigned long) out->boundaryfacecount);
  printf("  Mesh exterior boundary edges: %lu\n",
         (unsigned long) (3 * out->boundaryfacecount / 2));
  if (behave->poly) {
    printf("  Mesh interior boundary faces: %lu\n",
           (unsigned long) (outsubfacs - out->boundaryfacecount));
    printf("  Mesh subfacets (constrained faces): %lu\n",
           (unsigned long) outsubfacs);
    printf("  Mesh subsegments (constrained edges): %lu\n\n",
           (unsigned long) outsubsegs);
  } else {
    printf("\n");
  }
  if (behave->verbose) {
    statisticsquality(behave, in, plex);
    printf("Memory allocation statistics:\n\n");
    printf("  Maximum number of vertices: %lu\n",
           (unsigned long) plex->vertexpool->maxobjects);
    printf("  Maximum number of tetrahedron molecules: %lu\n",
           (unsigned long) plex->moleculepool.maxobjects);
    printf("  Memory for vertices (bytes): %lu\n",
           (unsigned long) proxipoolbytes(plex->vertexpool));
    printf("  Memory for tetrahedron molecules (bytes): %lu\n",
           (unsigned long) proxipoolbytes(&plex->moleculepool));
    printf("  Memory for star pointers (bytes): %lu\n",
           (unsigned long) arraypoolbytes(&plex->stars));
    printf("  Approximate heap memory use (bytes): %lu\n\n", (unsigned long)
           (proxipoolbytes(plex->vertexpool) + tetcomplexbytes(plex)));

    printf("Algorithmic statistics:\n\n");
    if (!behave->weighted) {
      printf("  Number of insphere tests: %lu\n",
             (unsigned long) behave->inspherecount);
    } else {
      printf("  Number of 4D orientation tests: %lu\n",
             (unsigned long) behave->orient4dcount);
    }
    printf("  Number of 3D orientation tests: %lu\n",
           (unsigned long) behave->orientcount);
    if (behave->tetcircumcentercount > 0) {
      printf("  Number of tetrahedron circumcenter computations: %lu\n",
             (unsigned long) behave->tetcircumcentercount);
    }
    if (behave->tricircumcentercount > 0) {
      printf("  Number of triangle circumcenter computations: %lu\n",
             (unsigned long) behave->tricircumcentercount);
    }
    printf("\n");
  }
}

/**                                                                         **/
/**                                                                         **/
/********* Statistics printing routines end here                     *********/


