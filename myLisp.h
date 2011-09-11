#ifndef _MYLISP_H_
#define _MYLISP_H_

#define CAR(e) (((LVALUE*) e)->u.cell.car)
#define CDR(e) (((LVALUE*) e)->u.cell.cdr)

#define LTEST(v) (((VALUE)(v) & ~Qnil) != 0)
#define NIL_P(v) ((VALUE)(v) == Qnil)

#define FALSE_P(v)  ((VALUE)(v) == Qfalse)
#define TRUE_P(v)  ((VALUE)(v) == Qtrue)
#define SYMBOL_P(e) (((LVALUE*) e)->type == SYMBOL)
#define PAIR_P(e) (((LVALUE*)e)->type == CELL)
#define CLOSURE_P(e) (((LVALUE*)e)->type == CLOSURE)
#define MACRO_P(e) (((LVALUE*)e)->type == MACRO)
#define SYMBOL_NAME(e) (((LVALUE*)e)->u.symbol)
typedef unsigned long VALUE;
typedef struct LVALUE_tag LVALUE;

#define Qfalse (VALUE)0
#define Qtrue (VALUE)2
#define Qnil (VALUE)4
#define Qundef (VALUE)6

#define FIXNUM_FLAG 0x01
#define FIXNUM_P(f) (((int)(f))&FIXNUM_FLAG)
#define INT2FIX(i) ((VALUE)(((int)(i))<<1 | FIXNUM_FLAG))
#define RSHIFT(x,y) ((x)>>y)
#define FIX2INT(x) RSHIFT((int)x,1)

#define GETPARAMS(e) (((LVALUE*)e)->u.closure.params)
#define GETE(v) (((LVALUE*)v)->u.closure.e)

#define DIRECTVAL_P(e) (FALSE_P(e) || TRUE_P(e) || NIL_P(e) || FIXNUM_P(e))
typedef enum Type_tag {
  SYMBOL,
  CELL,
  CLOSURE,
  MACRO
} Type;

typedef struct Cell_tag
{
  VALUE car;
  VALUE cdr;
} Cell;

typedef struct Closure_tag
{
  VALUE params;
  VALUE e;
} Closure;

struct LVALUE_tag
{
  Type type;
  union
  {
    char *symbol;
    Cell cell;
    Closure closure;
  }u;
};


VALUE make_atom(Type, char*);
VALUE cons(VALUE, VALUE);
VALUE append(VALUE, VALUE);
void print_tree(VALUE);
void prompt();
VALUE eval(VALUE, VALUE);
VALUE apply(VALUE, VALUE, VALUE);
VALUE assoc(VALUE, VALUE);
VALUE pairlis(VALUE, VALUE);

VALUE procedure_car(VALUE, VALUE);
VALUE procedure_cdr(VALUE, VALUE);
VALUE procedure_cons(VALUE, VALUE);
VALUE quote(VALUE, VALUE);
VALUE eq(VALUE, VALUE);
VALUE atom(VALUE, VALUE);
VALUE add(VALUE, VALUE);
VALUE sub(VALUE, VALUE);
VALUE mul(VALUE, VALUE);
VALUE divide(VALUE, VALUE);
VALUE mod(VALUE, VALUE);
VALUE cond(VALUE, VALUE);
VALUE define(VALUE, VALUE);
VALUE define_macro(VALUE, VALUE);
VALUE lambda(VALUE, VALUE);

VALUE topenv;
#endif /* _MYLISP_H_ */
