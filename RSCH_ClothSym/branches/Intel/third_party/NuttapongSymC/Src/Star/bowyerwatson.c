/********* Bowyer-Watson Delaunay insertion routines begin here      *********/
/**                                                                         **/
/**                                                                         **/

void bowyerwatsonworkarrayinit(struct arraypool *trianglelist)
{
  arraypoolinit(trianglelist,
                (arraypoolulong) sizeof(struct tetcomplexlinktriangle),
                (arraypoolulong) 8, 0);
  arraypoolforcelookup(trianglelist, (arraypoolulong) 3);
}

void bowyerwatson(struct behavior *behave,
                  struct tetcomplex *plex,
                  struct arraypool *trianglelist,
                  tag newvertex,
                  tag vtx1,
                  tag vtx2,
                  tag vtx3,
                  tag vtx4,
                  tag returntriangle[3])
{
  starreal *point0, *point1, *point2, *point3, *pointnew, *pointfar;
  struct tetcomplexlinktriangle *triangle;
  struct tetcomplexlinktriangle *newtri1, *newtri2;
  struct tetcomplexlinktriangle *mirrortri;
  struct tetcomplexlinktriangle *othertri;
  struct tetcomplexlinktriangle *othertri2;
  tag adjacencies[2];
  tag adjacencies2[2];
  starreal ori;
  arraypoolulong activetricount;
  arraypoolulong delaunayfaceindex;
  arraypoolulong delaunayfacecount;
  arraypoolulong mirror;
  arraypoolulong neighbor;
  arraypoolulong neighbor2;
  arraypoolulong moveindex;
  int delaunayfaceflag;
  int firstnewtri;
  int edge;
  int edgesum;
  int mirroredge;

#ifdef SELF_CHECK
  if (newvertex >= STOP) {
    printf("Internal error in bowyerwatson():\n");
    printf("  New vertex tag is not a valid vertex tag.\n");
    internalerror();
  }
  if ((vtx1 == GHOSTVERTEX) + (vtx2 == GHOSTVERTEX) +
      (vtx3 == GHOSTVERTEX) + (vtx4 == GHOSTVERTEX) > 1) {
    printf("Internal error in bowyerwatson():\n");
    printf("  Tetrahedron has more than one ghost vertex.\n");
    internalerror();
  }
#endif /* SELF_CHECK */

  if (vtx1 == GHOSTVERTEX) {
    vtx1 = vtx2;
    vtx2 = vtx4;
    vtx4 = GHOSTVERTEX;
  } else if (vtx2 == GHOSTVERTEX) {
    vtx2 = vtx3;
    vtx3 = vtx4;
    vtx4 = GHOSTVERTEX;
  } else if (vtx3 == GHOSTVERTEX) {
    vtx3 = vtx1;
    vtx1 = vtx4;
    vtx4 = GHOSTVERTEX;
  }

  tetcomplex12flipon6edges(plex, vtx1, vtx2, vtx3, vtx4, newvertex);
  triangle = (struct tetcomplexlinktriangle *)
             arraypoolfastlookup(trianglelist, 0);
  triangle->vtx[0] = vtx2;
  triangle->vtx[1] = vtx3;
  triangle->vtx[2] = vtx4;
  triangle->neighbor[0] = 5;
  triangle->neighbor[1] = 8;
  triangle->neighbor[2] = 14;
  triangle = (struct tetcomplexlinktriangle *)
             arraypoolfastlookup(trianglelist, 1);
  triangle->vtx[0] = vtx3;
  triangle->vtx[1] = vtx1;
  triangle->vtx[2] = vtx4;
  triangle->neighbor[0] = 9;
  triangle->neighbor[1] = 0;
  triangle->neighbor[2] = 13;
  triangle = (struct tetcomplexlinktriangle *)
             arraypoolfastlookup(trianglelist, 2);
  triangle->vtx[0] = vtx1;
  triangle->vtx[1] = vtx2;
  triangle->vtx[2] = vtx4;
  triangle->neighbor[0] = 1;
  triangle->neighbor[1] = 4;
  triangle->neighbor[2] = 12;
  triangle = (struct tetcomplexlinktriangle *)
             arraypoolfastlookup(trianglelist, 3);
  triangle->vtx[0] = vtx3;
  triangle->vtx[1] = vtx2;
  triangle->vtx[2] = vtx1;
  triangle->neighbor[0] = 10;
  triangle->neighbor[1] = 6;
  triangle->neighbor[2] = 2;

  pointnew = ((struct vertex *) tetcomplextag2vertex(plex, newvertex))->coord;

  firstnewtri = 1;
  activetricount = 4;
  delaunayfaceindex = 0;
  delaunayfacecount = 0;
  do {
    activetricount--;
    triangle = (struct tetcomplexlinktriangle *)
               arraypoolfastlookup(trianglelist, activetricount);
    if (triangle->vtx[0] != STOP) {
      tetcomplexadjacencies(plex, triangle->vtx[0], triangle->vtx[1],
                            triangle->vtx[2], adjacencies);
      if (adjacencies[1] == newvertex) {
        tetcomplexsqueezetriangle(plex, triangle->vtx[0], triangle->vtx[1],
                                  triangle->vtx[2]);

        for (edge = 0; edge < 3; edge++) {
          mirror = triangle->neighbor[edge];
          mirrortri = (struct tetcomplexlinktriangle *)
                      arraypoolfastlookup(trianglelist, mirror >> 2);
          if (mirrortri->vtx[mirror & 3] == triangle->vtx[edge]) {
            edgesum = edge + (mirror & 3);
            break;
          }
        }
        if (edge == 3) {
          mirror = activetricount;
          while (mirror > 0) {
            mirror--;
            mirrortri = (struct tetcomplexlinktriangle *)
                        arraypoolfastlookup(trianglelist, mirror);
            for (edgesum = 2; edgesum < 5; edgesum++) {
              if ((triangle->vtx[0] ==
                   mirrortri->vtx[(edgesum == 2) ? 2 : edgesum - 3]) &&
                  (triangle->vtx[1] ==
                   mirrortri->vtx[(edgesum == 4) ? 0 : edgesum - 1]) &&
                  (triangle->vtx[2] == mirrortri->vtx[edgesum - 2])) {
                break;
              }
            }
            if (edgesum < 5) {
              break;
            }
            if (mirror == 0) {
              printf("Internal error in bowyerwatson():\n");
              printf("  Squeezed triangle has no mirror image.\n");
              internalerror();
            }
          }

          mirror = mirror << 2;
        }

        for (edge = 0; edge < 3; edge++) {
          neighbor = triangle->neighbor[edge];
          if ((neighbor >> 2) != (mirror >> 2)) {
            mirroredge = edgesum - edge;
            mirroredge = (mirroredge > 2) ? (mirroredge - 3) :
                         (mirroredge < 0) ? (mirroredge + 3) : mirroredge;
            othertri = (struct tetcomplexlinktriangle *)
                       arraypoolfastlookup(trianglelist, neighbor >> 2);
            neighbor2 = mirrortri->neighbor[mirroredge];
            othertri2 = (struct tetcomplexlinktriangle *)
                        arraypoolfastlookup(trianglelist, neighbor2 >> 2);
            othertri->neighbor[neighbor & 3] = neighbor2;
            othertri2->neighbor[neighbor2 & 3] = neighbor;
          }
        }
        mirrortri->vtx[0] = STOP;
      } else {
        point0 = ((struct vertex *)
                  tetcomplextag2vertex(plex, triangle->vtx[0]))->coord;
        point1 = ((struct vertex *)
                  tetcomplextag2vertex(plex, triangle->vtx[1]))->coord;

        if (triangle->vtx[2] == GHOSTVERTEX) {
#ifdef SELF_CHECK
          if (adjacencies[1] == GHOSTVERTEX) {
            printf("Internal error in bowyerwatson():\n");
            printf("  Tetrahedron has two ghost vertices.\n");
            internalerror();
          }
#endif /* SELF_CHECK */

          point3 = ((struct vertex *)
                    tetcomplextag2vertex(plex, adjacencies[1]))->coord;
          ori = orient3d(behave, pointnew, point0, point1, point3);
          if (ori != 0.0) {
            delaunayfaceflag = ori < 0.0;
          } else {
            if (!tetcomplexadjacencies(plex,
                                       triangle->vtx[0], triangle->vtx[1],
                                       adjacencies[1], adjacencies2)) {
              printf("Internal error in bowyerwatson():\n");
              printf("  Triangular face is present yet not present.\n");
              internalerror();
            }
            if (adjacencies2[1] == GHOSTVERTEX) {
              printf("Internal error in bowyerwatson():\n");
              printf("  Triangular face has ghost vertex on both sides.\n");
              internalerror();
            }

            if (adjacencies2[1] == newvertex) {
              delaunayfaceflag = 0;
            } else {
              pointfar = ((struct vertex *)
                          tetcomplextag2vertex(plex, adjacencies2[1]))->coord;
              delaunayfaceflag = nonregular(behave, point0, point1, point3,
                                            pointfar, pointnew) <= 0.0;
            }
          }
        } else {
          point2 = ((struct vertex *)
                    tetcomplextag2vertex(plex, triangle->vtx[2]))->coord;
          if (adjacencies[1] == GHOSTVERTEX) {
            delaunayfaceflag = orient3d(behave, pointnew,
                                        point0, point1, point2) > 0.0;
          } else {
            point3 = ((struct vertex *)
                      tetcomplextag2vertex(plex, adjacencies[1]))->coord;
            delaunayfaceflag = nonregular(behave, point0, point1, point2,
                                          point3, pointnew) <= 0.0;
          }
        }

        if (delaunayfaceflag) {
#ifdef SELF_CHECK
          if (triangle->vtx[2] != GHOSTVERTEX) {
            vertexcheckorientation(behave, plex, newvertex, triangle->vtx[0],
                                   triangle->vtx[1], triangle->vtx[2]);
          }
#endif /* SELF_CHECK */

          if (delaunayfacecount > 0) {
            if (activetricount < delaunayfaceindex - 1) {
              othertri = (struct tetcomplexlinktriangle *)
                arraypoolfastlookup(trianglelist, delaunayfaceindex - 1);
              *othertri = *triangle;
              for (edge = 0; edge < 3; edge++) {
                neighbor = triangle->neighbor[edge];
                othertri = (struct tetcomplexlinktriangle *)
                  arraypoolfastlookup(trianglelist, neighbor >> 2);
                othertri->neighbor[neighbor & 3] =
                  ((delaunayfaceindex - 1) << 2) + edge;
              }
            }
            delaunayfaceindex--;
          } else {
            delaunayfaceindex = activetricount;
          }
          delaunayfacecount++;

          /*
          tetcomplexinserttetontripod(plex, newvertex, triangle->vtx[0],
                                      triangle->vtx[1], triangle->vtx[2]);
          */

          if (firstnewtri) {
            returntriangle[0] = triangle->vtx[0];
            returntriangle[1] = triangle->vtx[1];
            returntriangle[2] = newvertex;
            firstnewtri = 0;
          }
        } else {
          tetcomplexdeletetriangle(plex, triangle->vtx[0], triangle->vtx[1],
                                   triangle->vtx[2]);
          tetcomplex12flipon3edges(plex, adjacencies[1], triangle->vtx[2],
                                   triangle->vtx[1], triangle->vtx[0],
                                   newvertex);

          newtri2 = (struct tetcomplexlinktriangle *)
                    arraypoolforcelookup(trianglelist, activetricount + 2);
          newtri1 = (struct tetcomplexlinktriangle *)
                    arraypoolfastlookup(trianglelist, activetricount + 1);

          if (delaunayfaceindex == activetricount + 1) {
            if (delaunayfacecount == 1) {
              moveindex = delaunayfaceindex + 2;
              delaunayfaceindex += 2;
            } else {
              moveindex = delaunayfaceindex + delaunayfacecount;
              delaunayfaceindex++;
            }
            othertri = (struct tetcomplexlinktriangle *)
                       arraypoolforcelookup(trianglelist, moveindex);
            *othertri = *newtri1;
            for (edge = 0; edge < 3; edge++) {
              neighbor = newtri1->neighbor[edge];
              othertri = (struct tetcomplexlinktriangle *)
                arraypoolfastlookup(trianglelist, neighbor >> 2);
              othertri->neighbor[neighbor & 3] = (moveindex << 2) + edge;
            }
          }
          if (delaunayfaceindex == activetricount + 2) {
            othertri = (struct tetcomplexlinktriangle *)
                       arraypoolforcelookup(trianglelist, delaunayfaceindex +
                                            delaunayfacecount);
            *othertri = *newtri2;
            for (edge = 0; edge < 3; edge++) {
              neighbor = newtri2->neighbor[edge];
              othertri = (struct tetcomplexlinktriangle *)
                arraypoolfastlookup(trianglelist, neighbor >> 2);
              othertri->neighbor[neighbor & 3] =
                ((delaunayfaceindex + delaunayfacecount) << 2) + edge;
            }
            delaunayfaceindex++;
          }

          if (triangle->vtx[2] == GHOSTVERTEX) {
            newtri2->vtx[0] = triangle->vtx[0];
            newtri2->vtx[1] = triangle->vtx[1];
            newtri2->vtx[2] = adjacencies[1];
            newtri1->vtx[0] = triangle->vtx[0];
            newtri1->vtx[1] = adjacencies[1];
            newtri1->vtx[2] = triangle->vtx[2];
            triangle->vtx[0] = adjacencies[1];

            neighbor = triangle->neighbor[2];
            othertri = (struct tetcomplexlinktriangle *)
                       arraypoolfastlookup(trianglelist, neighbor >> 2);
            othertri->neighbor[neighbor & 3] = (activetricount << 2) + 10;
            newtri2->neighbor[0] = (activetricount << 2) + 2;
            newtri2->neighbor[1] = (activetricount << 2) + 6;
            newtri2->neighbor[2] = neighbor;
            neighbor = triangle->neighbor[1];
            othertri = (struct tetcomplexlinktriangle *)
                       arraypoolfastlookup(trianglelist, neighbor >> 2);
            othertri->neighbor[neighbor & 3] = (activetricount << 2) + 5;
            newtri1->neighbor[0] = (activetricount << 2) + 1;
            newtri1->neighbor[1] = neighbor;
            newtri1->neighbor[2] = (activetricount << 2) + 9;
            /* triangle->neighbor[0] is still connected correctly. */
            triangle->neighbor[1] = (activetricount << 2) + 4;
            triangle->neighbor[2] = (activetricount << 2) + 8;
          } else {
            newtri2->vtx[0] = triangle->vtx[0];
            newtri2->vtx[1] = triangle->vtx[1];
            newtri2->vtx[2] = adjacencies[1];
            newtri1->vtx[0] = triangle->vtx[1];
            newtri1->vtx[1] = triangle->vtx[2];
            newtri1->vtx[2] = adjacencies[1];
            triangle->vtx[1] = triangle->vtx[0];
            triangle->vtx[0] = triangle->vtx[2];
            triangle->vtx[2] = adjacencies[1];

            neighbor = triangle->neighbor[2];
            othertri = (struct tetcomplexlinktriangle *)
                       arraypoolfastlookup(trianglelist, neighbor >> 2);
            othertri->neighbor[neighbor & 3] = (activetricount << 2) + 10;
            newtri2->neighbor[0] = (activetricount << 2) + 5;
            newtri2->neighbor[1] = activetricount << 2;
            newtri2->neighbor[2] = neighbor;
            neighbor = triangle->neighbor[0];
            othertri = (struct tetcomplexlinktriangle *)
                       arraypoolfastlookup(trianglelist, neighbor >> 2);
            othertri->neighbor[neighbor & 3] = (activetricount << 2) + 6;
            newtri1->neighbor[0] = (activetricount << 2) + 1;
            newtri1->neighbor[1] = (activetricount << 2) + 8;
            newtri1->neighbor[2] = neighbor;
            neighbor = triangle->neighbor[1];
            othertri = (struct tetcomplexlinktriangle *)
                       arraypoolfastlookup(trianglelist, neighbor >> 2);
            othertri->neighbor[neighbor & 3] = (activetricount << 2) + 2;
            triangle->neighbor[0] = (activetricount << 2) + 9;
            triangle->neighbor[1] = (activetricount << 2) + 4;
            triangle->neighbor[2] = neighbor;
          }
          activetricount += 3;
        }
      }
    }
  } while (activetricount > 0);

  tetcomplexbuild3dstar(plex, newvertex, trianglelist,
                        delaunayfaceindex, delaunayfacecount);

  /*^^^ Assertion:  The tetrahedral complex is now consistent. */
#ifdef PARANOID
  tetcomplexconsistency(plex);
#endif /* PARANOID */

}

/**                                                                         **/
/**                                                                         **/
/********* Bowyer-Watson Delaunay insertion routines end here        *********/


