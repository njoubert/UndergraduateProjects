/******** Constraining segment/facet routines begin here              ********/
/**                                                                         **/
/**                                                                         **/

struct segment *constrainreadrecoversegments(struct behavior *behave,
                                             struct inputs *in,
                                             FILE *polyfile,
                                             struct proxipool *vertexpool,
                                             struct outputs *out,
                                             struct tetcomplex *plex)
{
  struct segment *segmentlist;
  arraypoolulong i;
  int markflag;

  markflag = inputsegmentheader(behave, in, polyfile);
  if (in->segmentcount == 0) {
    return (struct segment *) NULL;
  }

  segmentlist = (struct segment *)
    starmalloc((size_t) (in->segmentcount * sizeof(struct segment)));

  for (i = 0; i < in->segmentcount; i++) {
    inputsegment(behave, in, polyfile, i, markflag, &segmentlist[i]);
  }

  return segmentlist;
}

/**                                                                         **/
/**                                                                         **/
/******** Constraining segment/facet routines end here                ********/


