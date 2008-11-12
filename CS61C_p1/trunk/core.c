/*
 *  You are not required to understand the code in this file
 *  Do NOT change anything in this file! 
*/

#include "microscheme.h"
char* quote = "quote";

/*********************************************************************
 *  Constructor definitions
 *********************************************************************/

ThingPtr cons(ThingPtr a, ThingPtr b) {
  ThingPtr new;

  new = (ThingPtr)malloc(sizeof(struct thing));
  new->th_type = PAIR;
  new->th_car = a;
  new->th_cdr = b;
  return(new);
}

ThingPtr make_symbol(char *str) {
  ThingPtr new;

  new = (ThingPtr)malloc(sizeof(struct thing));
  new->th_type = SYMBOL;
  new->th_symbol = str;
  return(new);
}

ThingPtr quoted(ThingPtr th) {
  return(cons(make_symbol(quote),cons(th,NIL)));
}

/*********************************************************************
 *  Selector definitions
 *********************************************************************/

ThingPtr car(ThingPtr pair) {
  if (pair == NIL || pair->th_type != PAIR) {
    printf("Error: car of non-pair.\n");
    return(NIL);
  }
  return(pair->th_car);
}

ThingPtr cdr(ThingPtr pair) {
  if (pair == NIL || pair->th_type != PAIR) {
    printf("Error: cdr of non-pair.\n");
    return(NIL);
  }
  return(pair->th_cdr);
}

/*********************************************************************
 *  Memory Mangement definitions
 *********************************************************************/

ThingPtr car_with_free(ThingPtr pair) {
  ThingPtr retVal;
  if (pair == NIL || pair->th_type != PAIR) {
    printf("Error: car of non-pair.\n");
    return(NIL);
  }

  retVal = pair->th_car;
  free_thing(pair->th_cdr,0);
  free(pair);
  return retVal;
}

ThingPtr cdr_with_free(ThingPtr pair) {
  ThingPtr retVal;
  if (pair == NIL || pair->th_type != PAIR) {
    printf("Error: cdr of non-pair.\n");
    return(NIL);
  }
  retVal = pair->th_cdr;
  free_thing(pair,1);
  return retVal;
}

//Generic free code
/*pair delete level:
  0: Delete all pairs
  1: Keep th_cdr; remove th_car (at delete level 0) and th
  2: Keep all th_car; remove all th_cdr (at delete level 2) and th
*/
void free_thing(ThingPtr th, int pair_delete_level) {
  if (th == NIL)
    return;
  if (th->th_type == SYMBOL){
    if (th->th_symbol != quote)
      free(th->th_symbol);
  }
  else if (th->th_type == NUMBER){
    free_number(th);
  }
  else if (th->th_type == PAIR){
    if (pair_delete_level == 2){
      free_thing(th->th_cdr,pair_delete_level);
    } else if (pair_delete_level == 1){
      free_thing(th->th_car,0);
    }	else{
      free_thing(th->th_car,0);
      free_thing(th->th_cdr,0);
    }
  }
  free (th);
}

/*********************************************************************
 *  Define printing functions
 *********************************************************************/

void printthing(ThingPtr th) {
  if (th == NIL)
    printf("()");
  else switch (th->th_type) {
  case SYMBOL:
    printf("%s",th->th_symbol);
    break;
  case NUMBER:
    print_number(th);
    break;
  case PAIR:
    printf("(");
    printthing(car(th));
    printtail(cdr(th));
    break;
  }
}

void printtail(ThingPtr th) {
  if (th == NIL)
    printf(")");
  else switch (th->th_type) {
  case SYMBOL:
    printf(" . %s)",th->th_symbol);
    break;
  case NUMBER:
    printf(" . ");
    print_number(th);
    printf(")");
    break;
  case PAIR:
    printf(" ");
    printthing(car(th));
    printtail(cdr(th));
    break;
  }
}

/*********************************************************************
 *  Define reading functions
 *********************************************************************/

ThingPtr readlist(void) {
  ThingPtr new;
  char ch;

  while (isspace(ch = getchar())) ;
  if (ch == ')')
    return(NIL);
  ungetc(ch,stdin);
  new = readthing();
  if (new != NIL && new->th_type == SYMBOL && !strcmp(new->th_symbol,".")) {
    free_thing(new,0);
    new = readthing();
    ch = getchar();
    if (ch != ')')
      printf("Error in dot notation.\n");
    return(new);
  }
  return(cons(new,readlist()));
}
