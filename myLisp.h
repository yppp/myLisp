#ifndef _MYLISP_H_
#define _MYLISP_H_

#define CAR(e) (e->u.cell.car)
#define CDR(e) (e->u.cell.cdr)

#define NIL_P(e) (e->type == NIL)
#define INT_P(e) (e->type == INT)
#define SYMBOL_P(e) (e->type == SYMBOL)
#define PAIR_P(e) (e->type == CELL)

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
LVALUE* cons(LVALUE*, LVALUE*);
LVALUE* append(LVALUE*, LVALUE*);
void print_tree(LVALUE*);
void prompt();
#endif /* _MYLISP_H_ */
