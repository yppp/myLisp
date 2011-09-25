#ifndef _MYLISP_H_
#define _MYLISP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define CAR(e) (((LVALUE*) e)->u.cell.car)
#define CDR(e) (((LVALUE*) e)->u.cell.cdr)

#define LTEST(v) (((VALUE)(v) & ~Qnil) != 0)
#define NIL_P(v) ((VALUE)(v) == Qnil)

#define FALSE_P(v)  ((VALUE)(v) == Qfalse)
#define TRUE_P(v)  ((VALUE)(v) == Qtrue)
#define SYMBOL_P(e) (((LVALUE*) e)->u.basic.type == SYMBOL)
#define PAIR_P(e) (((LVALUE*)e)->u.basic.type == CELL)
#define CLOSURE_P(e) (((LVALUE*)e)->u.basic.type == CLOSURE)
#define MACRO_P(e) (((LVALUE*)e)->u.basic.type == MACRO)
#define NATIVE_PROCEDURE_P(e) (((LVALUE*)e)->u.basic.type == NATIVE_PROCEDURE)
#define SYMBOL_NAME(e) (((LVALUE*)e)->u.symbol.symbol)
typedef unsigned long VALUE;
typedef struct LVALUE_tag LVALUE;

#define Qfalse ((VALUE)0)
#define Qtrue ((VALUE)2)
#define Qnil ((VALUE)4)
#define Qundef ((VALUE)6)

#define FIXNUM_FLAG 0x01
#define FIXNUM_P(f) (((long)(f)) & FIXNUM_FLAG)
#define INT2FIX(i) ((VALUE)(((long)(i))<<1 | FIXNUM_FLAG))
#define RSHIFT(x,y) ((x)>>y)
#define FIX2INT(x) RSHIFT((long)x,1)

#define GETPARAMS(e) (((LVALUE*)e)->u.closure.params)
#define GETE(v) (((LVALUE*)v)->u.closure.e)

#define DIRECTVAL_P(e) (FALSE_P(e) || TRUE_P(e) || NIL_P(e) || FIXNUM_P(e))

#define SLOT_SIZE 1
#define HEAP_GROW 1

#define NELEMS(arr) (sizeof(arr) / sizeof(arr[0]))
typedef VALUE (*Subr)(VALUE, VALUE);

typedef enum Type_tag {
  FREE,
  SYMBOL,
  CELL,
  CLOSURE,
  MACRO,
  NATIVE_PROCEDURE
} Type;

typedef struct LBasic_tag
{
  long gc_mark;
  Type type;
} LBasic;

typedef struct Cell_tag
{
  LBasic basic;
  VALUE car;
  VALUE cdr;
} Cell;

typedef struct Symbol_tag
{
  LBasic basic;
  char *symbol;
} Symbol;

typedef struct Closure_tag
{
  LBasic basic;
  VALUE params;
  VALUE e;
} Closure;

typedef struct Native_tag
{
  LBasic basic;
  Subr proc;
} Native;

struct LVALUE_tag
{
  union
  {
    struct
    {
      LBasic basic;
      struct LVALUE_tag *next;
    } free;
    LBasic basic;
    Symbol symbol;
    Cell cell;
    Closure closure;
    Native native;
  }u;
};

typedef struct Heapslot_tag
{
  LVALUE values[SLOT_SIZE];
} Heapslot;


LVALUE* make_obj(VALUE);
void gc(VALUE);
void recursive_mark(LVALUE*);

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
void defsubr(char*, Subr);
VALUE make_symbol(char*, VALUE);
VALUE evlis(VALUE, VALUE);

VALUE topenv;
LVALUE *freelist;
struct
{
  long len;
  Heapslot **slots;
} heap;

#endif /* _MYLISP_H_ */
