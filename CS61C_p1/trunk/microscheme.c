/*
 *  Name: Niels Joubert
 *  Section:
 *  TA: Alex Kronrod
 *
 *  Your proj1 code will go into this file
 */

#include "microscheme.h"

/*********************************************************************
 *  Constructor definitions
 *********************************************************************/

// Re-implement Me
ThingPtr make_number(int num) {
  ThingPtr new;

  new = (ThingPtr)malloc(sizeof(struct thing));
  new->th_type = NUMBER;
  new->th_number = num;
  return(new);
}

/*********************************************************************
 *  Memory Mangement definitions
 *********************************************************************/

void free_number(ThingPtr th)
{
  // Implement Me!
}

/*********************************************************************
 *  Define printing functions
 *********************************************************************/

void print_number(ThingPtr th)
{
  // Re-implement Me!
  printf("%d",th->th_number);
}

/*********************************************************************
 *  Define reading functions
 *********************************************************************/

ThingPtr readthing(void) {
  char ch, *new;
  char buffer[30];  // accepts only 30 characters of input

  while (isspace(ch = getchar())) ;
  if (ch == '(')
    return(readlist());
  if (ch == '\'')
    return(quoted(readthing()));
  if (ch == ')') {
    printf("Unmatched close paren.\n");
    return(NIL);
  }
  ungetc(ch, stdin);
  scanf("%29[^() \n\t]",buffer);
  if (isNumber(buffer))
    return(make_number(atoi(buffer)));
  new = (char *)malloc(1+strlen(buffer));
  strcpy(new,buffer);
  return(make_symbol(new));
}

boolean isNumber(char *str) {
  char ch = *str;

  if (ch == '-')
    str++;
  while ((ch = *str++)) {
    if (ch < '0' || ch > '9')
      return(FALSE);
  }
  return(TRUE);
}

/*********************************************************************
 *  Define primitives, such as arithmetic primitives
 *********************************************************************/

ThingPtr plus(ThingPtr a, ThingPtr b) {
  ThingPtr retval;
	if (a == NIL || a->th_type != NUMBER || b == NIL || b->th_type != NUMBER) {
    printf("Non-numeric arg to plus.\n");
    return(NIL);
  }
  retval = make_number(a->th_number + b->th_number);
  free_thing(a,0);
  free_thing(b,0);
  return retval;
}

//op-codes
#define PLUS 1

ThingPtr handleArithmeticOp(char* op, ThingPtr argList) {
  ThingPtr answer, evl;
	// convert operators to "op-codes"
	int opcode; //+ is 1;

	// add additional opcodes
	if (!strcmp(op,"+"))
		opcode=1;
	else{ /*error with 0*/
     for (evl = argList;evl != NULL; evl=evl->th_cdr)
			free_thing(evl,0);
		return make_number(0);
	}

  if (argList == NIL) { //no operation
		answer = (ThingPtr) malloc(sizeof(struct thing));
		answer->th_type = NUMBER;

		// add support for other operators with no args here.
		// if it's illegal to call the operator with no args,
		// print an error.
		if (opcode==PLUS) {
	    answer->th_number = 0;
		} else {
	    answer = NIL;
	    printf("Bad Else in handleArithmeticOp\n");
		}

		return answer;
  }

	//BASE
	if (opcode == PLUS) {
	    answer = eval(car(argList));
	}
		//add additional operators here


  //Generic loop to process operations
	//Add support for other operators here
	for (evl = argList->th_cdr;evl != NULL; evl=evl->th_cdr){
		if (opcode == PLUS) {
	    answer = plus(answer, eval(car(evl)));
		}
		//add support for other operations here
	}

	return answer;
}

/*********************************************************************
 *  Define the evaluation loop
 *********************************************************************/

ThingPtr eval(ThingPtr exp) {
  char *fn;
  ThingPtr retVal = NIL;

  if (exp == NIL)
    return(NIL);
  switch(exp->th_type) {
  case NUMBER:
    return(exp);
  case SYMBOL:
    free_thing(exp,0);
    printf("Sorry, but variables are not supported in microscheme\n");
    return(NIL);
  case PAIR:   //function application is here
    if ((car(exp))->th_type != SYMBOL) {
      free_thing(exp,0);
      printf("Application of non-procedure\n");
      return(NIL);
    }
    fn = (car(exp))->th_symbol;
    if (!strcmp(fn,quote)){
      retVal = cdr(exp);
      free_thing(exp,1);
      return car_with_free(retVal);
    }
    if (!strcmp(fn,"+")){
      retVal = handleArithmeticOp(fn, cdr(exp));
      free_thing(car(exp),0);
      free_thing(exp, 2);  //all sub-expressions are now gone
			return retVal;
    }

    // Add more procedure support here

    if (!strcmp(fn,"car")){
      if (cdr (exp) == NIL){
	printf("car with no arguments\n");
	free_thing(exp,0);
	return (NIL);
      }
      if (cdr (cdr(exp)) != NIL){
	printf("car with too many arguments\n");
	free_thing(exp,0);
	return (NIL);
      }
      retVal = (eval(cadr(exp)));
      free_thing(car(exp),0);           //remove "car" symbol
      free_thing(exp,2);                //remove top level list
      return car_with_free(retVal);     //return car of retVal
    }
    if (!strcmp(fn,"cdr")){
      if (cdr (exp) == NIL){
	printf("cdr with no arguments\n");
	free_thing(exp,FALSE);
	return (NIL);
      }
      if (cdr (cdr(exp)) != NIL){
	printf("cdr with too many arguments\n");
	free_thing(exp,FALSE);
	return (NIL);
      }
      retVal = (eval(cadr(exp)));
      free_thing(car(exp),0);  //remove "cdr" symbol
      free_thing(exp,2);			//remove top level list
      //return cdr of retVal
      return cdr_with_free(retVal);
    }
    if (!strcmp(fn,"cons")){
      if ((cdr (exp) == NIL) || (cdr (cdr(exp)) == NIL)){
	printf("cons with insufficient arguments\n");
	free_thing(exp,0);
	return (NIL);
      }
      if (cdr (cdr(cdr(exp))) != NIL){
	printf("cons with too many arguments\n");
	free_thing(exp,FALSE);
	return (NIL);
      }
      retVal = cons(eval(cadr(exp)),eval(caddr(exp)));
      free_thing(car(exp),0); //remove "cons" symbol
      free_thing(exp,2);  //remove top level list
      return retVal;
    }
    if (!strcmp(fn,"exit")){
      free_thing(exp,0);
      doMemCheck();
      exit(0);
    }
    printf("Unknown function\n");
    free_thing(exp,0);
    return(NIL);
  }
  return 0;
}

/* read-eval-print loop */

int main() {
  for (;;) {
    ThingPtr evaluated;
    printf("> ");
    fflush(stdout);
    evaluated = eval(readthing());
    printthing(evaluated);
    free_thing(evaluated,FALSE);
    printf("\n");
  }
  doMemCheck();
  return 0;
}

