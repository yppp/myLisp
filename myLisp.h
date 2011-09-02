#ifndef _MYLISP_H_
#define _MYLISP_H_

typedef unsigned long VALUE;
typedef struct LVALUE_tag LVALUE;

typedef enum Type_tag {
  NIL,
  INT,
  SYMBOL,
  CELL
} Type;

typedef struct Cell_tag
{
  LVALUE *car;
  LVALUE *cdr;
} Cell;

struct LVALUE_tag
{
  Type type;
  union
  {
    int integer;
    char *symbol;
    Cell cell;
  }u;
};




LVALUE* make_atom(Type, char*);
LVALUE* car(LVALUE*);
LVALUE* cdr(LVALUE*);
LVALUE* cons(LVALUE*, LVALUE*);
LVALUE* append(LVALUE*, LVALUE*);
void printtree(LVALUE*);
#endif /* _MYLISP_H_ */
