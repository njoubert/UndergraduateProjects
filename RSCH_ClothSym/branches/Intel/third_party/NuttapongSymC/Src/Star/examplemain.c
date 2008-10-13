int main(int argc,
         char **argv)
{
  struct proxipool vertexpool;
  struct tetcomplex mesh;
  struct behavior behave;
  struct vertex *vertexptr1, *vertexptr2, *vertexptr3, *vertexptr4;
  tag tag1, tag2, tag3, tag4;
  starreal *point1, *point2, *point3, *point4;

  /* Example 1:  Using a tetcomplex with no vertex data structures. */
  printf("\nExample 1.\n");

  /* Initialize the tetcomplex; make it highly verbose (4). */
  tetcomplexinit(&mesh, (struct proxipool *) NULL, 4);

  /* Insert two (oriented) tetrahedra. */
  tetcomplexinserttet(&mesh, 0, 1, 2, 3);
  tetcomplexinserttet(&mesh, 1, 2, 3, 4);
  /* Flip one tetrahedron to four by lazily inserting vertex 5. */
  tetcomplex14flip(&mesh, 0, 1, 2, 3, 5);
  /* Flip two tetrahedra to three. */
  tetcomplex23flip(&mesh, 5, 1, 2, 3, 4);

  /* Print the mesh and make sure the data structure is not corrupted. */
  tetcomplexprint(&mesh);
  tetcomplexconsistency(&mesh);

  /* Reverse the last two flips. */
  tetcomplex32flip(&mesh, 5, 1, 2, 3, 4);
  tetcomplex41flip(&mesh, 0, 1, 2, 3, 5);
  /* Delete a tetrahedron, leaving only one. */
  tetcomplexdeletetet(&mesh, 1, 2, 3, 4);

  /* Print the mesh and make sure the data structure is not corrupted. */
  tetcomplexprint(&mesh);
  tetcomplexconsistency(&mesh);

  /* Free the mesh. */
  tetcomplexdeinit(&mesh);


  /* Example 2:  With vertex data structures. */
  printf("\nExample 2.\n");

  /* Create a pool of data structures of type `struct vertex', with no */
  /*   auxiliary data.  Make the pool highly verbose (4).              */
  proxipoolinit(&vertexpool, sizeof(struct vertex), (size_t) 0, 4);
  /* Initialize the primitives so we can do orientation tests. */
  primitivesinit();
  /* Get the command line switches and initialize the operation count. */
  parsecommandline(argc, argv, &behave);

  /* Allocate and initialize four vertices. */
  tag1 = proxipoolnew(&vertexpool, 0, (void **) &vertexptr1);
  vertexptr1->coord[0] = 3.0;
  vertexptr1->coord[1] = 2.5;
  vertexptr1->coord[2] = 1.1;
  vertexptr1->mark = 0;
  vertexptr1->number = 1;
  point1 = vertexptr1->coord;
  tag2 = proxipoolnew(&vertexpool, 0, (void **) &vertexptr2);
  vertexptr2->coord[0] = 4.4;
  vertexptr2->coord[1] = 7.2;
  vertexptr2->coord[2] = 3.8;
  vertexptr2->mark = 0;
  vertexptr2->number = 2;
  point2 = vertexptr2->coord;
  tag3 = proxipoolnew(&vertexpool, 0, (void **) &vertexptr3);
  vertexptr3->coord[0] = 5.3;
  vertexptr3->coord[1] = 1.7;
  vertexptr3->coord[2] = 2.9;
  vertexptr3->mark = 0;
  vertexptr3->number = 3;
  point3 = vertexptr3->coord;
  tag4 = proxipoolnew(&vertexpool, 0, (void **) &vertexptr4);
  vertexptr4->coord[0] = 0.5;
  vertexptr4->coord[1] = 3.3;
  vertexptr4->coord[2] = 9.2;
  vertexptr4->mark = 0;
  vertexptr4->number = 4;
  point4 = vertexptr4->coord;
  printf("Orient:  %g\n", orient3d(&behave, point1, point2, point3, point4));

  /* Initialize the tetcomplex; make it highly verbose (4). */
  tetcomplexinit(&mesh, (struct proxipool *) NULL, 4);

  /* Insert a tetrahedron. */
  tetcomplexinserttet(&mesh, tag1, tag2, tag3, tag4);

  /* Print the mesh and make sure the data structure is not corrupted. */
  tetcomplexprint(&mesh);
  tetcomplexconsistency(&mesh);

  /* Free the pool of vertices. */
  proxipooldeinit(&vertexpool);
  /* Free the mesh. */
  tetcomplexdeinit(&mesh);

  return 0;
}
