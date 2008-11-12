/*********************************************************************
 *       Thing definition for microscheme
 *********************************************************************/

struct thing {
  char th_type;
  union {
    char *thu_symbol;
    int thu_number;    // Change the type of this line
    struct {struct thing *thup_car, *thup_cdr;} thu_pair;
  } th_union;
};
