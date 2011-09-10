#include "myLisp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VALUE  make_atom(Type type, char *str)
{
  LVALUE *r = (LVALUE*)malloc (sizeof(LVALUE));
  r->type = type;
  r->u.cell.car = Qnil;
  r->u.cell.cdr = Qnil;

  char *cp = NULL;

  if(str != NULL)
    {
      cp = (char*)malloc (sizeof(char) * (strlen(str) + 1));
      strcpy(cp, str);
    }

  switch(type)
    {
    case SYMBOL:
      r->u.symbol = cp;
      break;
    case CELL:
      break;
    }
  return (VALUE)r;
}

VALUE cons(VALUE car, VALUE cdr)
{
  LVALUE *cell = (LVALUE*)malloc(sizeof(LVALUE));
  cell->type = CELL;
  CAR(cell) = car;
  CDR(cell) = cdr;
  return (VALUE)cell;
}

VALUE append(VALUE lis, VALUE a)
{
  VALUE tmp;
  if (NIL_P(lis)) return a;

  for(tmp = lis; !(NIL_P(CDR(tmp))); tmp = CDR(tmp));

  CDR(tmp) = a;
  return lis;
}

int main(void)
{
  extern int yyparse();
  topenv = cons(cons(make_atom(SYMBOL, "___first"), Qnil), Qnil);
  
  prompt();
  while(yyparse());

  return 0;
}

void print_tree(VALUE tree)
{
  if(NIL_P(tree)) printf("()");
  else if(FALSE_P(tree)) printf("#f");
  else if(TRUE_P(tree)) printf("#t");
  else if(FIXNUM_P(tree)) printf("%d", FIX2INT(tree));
  else if(SYMBOL_P(tree)) printf("%s", SYMBOL_NAME(tree));
  else if (PAIR_P(tree))
    {
      printf("(");
      while(1)
	{
	  print_tree(CAR(tree));
	  tree = CDR(tree);
	  if(NIL_P(tree)) break;
	  
	  if(FIXNUM_P(tree) || SYMBOL_P(tree))
	    {
	      printf(" . ");
	      print_tree(tree);
	      break;
	    }
	  printf(" ");
	}
      printf(")");
    }
  else
    {
      fprintf(stderr, "print error");
      exit(1);
    }
}


void prompt()
{
  printf("myLisp> ");
}

VALUE  eval(VALUE tree, VALUE env)
{
  VALUE v;
  if (NIL_P(tree) || FIXNUM_P(tree)) return tree;
  else if (PAIR_P(tree) && SYMBOL_P(CAR(tree))) 
    {

      if(strcmp(SYMBOL_NAME(CAR(tree)), "car") == 0) return procedure_car(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "cdr") == 0) return procedure_cdr(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "cons") == 0) return procedure_cons(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "eq") == 0) return eq(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "atom") == 0) return atom(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "+") == 0) return add(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "-") == 0) return sub(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "*") == 0) return mul(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "/") == 0) return divide(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "%") == 0) return mod(CDR(tree), env);

      /* special form */
      if(strcmp(SYMBOL_NAME(CAR(tree)), "cond") == 0) return cond(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "lambda") == 0) return tree;
      if(strcmp(SYMBOL_NAME(CAR(tree)), "quote") == 0) return quote(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "define") == 0) return define(CDR(tree), env);
    }
  else if (SYMBOL_P(tree))
    {
      v = assoc(tree, env);
      if (!NIL_P(v)) return CDR(v);
      fprintf(stderr, "Undefined symbol '%s'", SYMBOL_NAME(tree));
      return Qnil;
    }

  return apply(CAR(tree), CDR(tree), env);
}

VALUE apply(VALUE func, VALUE args, VALUE  env)
{
  VALUE fbody = eval(func, env);
  VALUE ftype = CAR(fbody);

  if(!SYMBOL_P(ftype) || (SYMBOL_P(ftype) && strcmp(SYMBOL_NAME(ftype), "lambda") != 0))
    {
      fprintf(stderr, "invalid application ");
      print_tree(cons(func, args));
      return Qnil; 
    }


  VALUE params = CAR(CDR(fbody));
  VALUE e = CAR(CDR(CDR(fbody)));
  
  VALUE lis = args;
  VALUE eval_lis = Qnil;
  while(!NIL_P(lis))
    {
      eval_lis = append(eval_lis, cons(eval(CAR(lis), env), Qnil));
      lis = CDR(lis);
    }
  
  return eval(e, append(pairlis(params, eval_lis), env));
}

VALUE quote(VALUE args, VALUE env)
{
  return CAR(args);
}

VALUE  procedure_car(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE eval_lis = Qnil;
  while(!NIL_P(lis))
    {
      eval_lis = append(eval_lis, cons(eval(CAR(lis), env), Qnil));
      lis = CDR(lis);
    }

  return CAR(CAR(eval_lis));
}

VALUE  procedure_cdr(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE eval_lis = Qnil;
  while(!NIL_P(lis))
    {
      eval_lis = append(eval_lis, cons(eval(CAR(lis), env), Qnil));
      lis = CDR(lis);
    }

  return CDR(CAR(eval_lis));
}


VALUE eq(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE eval_lis = Qnil;
  while(!NIL_P(lis))
    {
      eval_lis = append(eval_lis, cons(eval(CAR(lis), env), Qnil));
      lis = CDR(lis);
    }

  return (CAR(eval_lis) == CAR(CDR(eval_lis))) ? Qtrue : Qfalse;
}

VALUE atom(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE eval_lis = Qnil;
  while(!NIL_P(lis))
    {
      eval_lis = append(eval_lis, cons(eval(CAR(lis), env), Qnil));
      lis = CDR(lis);
    }

  return FIXNUM_P(CAR(eval_lis))
    || NIL_P(CAR(eval_lis))
    || TRUE_P(CAR(eval_lis))
    || FALSE_P(CAR(eval_lis))
    || SYMBOL_P(CAR(eval_lis)) ? Qtrue : Qfalse;
}

VALUE add(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE evalret = Qnil;
  int acc = 0;

  while(!NIL_P(lis))
    {
      evalret = eval(CAR(lis), env);
      acc += FIXNUM_P(evalret) ? FIX2INT(evalret) : 0;
      lis = CDR(lis);
    }

  return INT2FIX(acc);
}

VALUE sub(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE evalret = Qnil;
  int acc = 0;

  evalret = eval(CAR(lis), env);
  acc = FIXNUM_P(evalret) ? FIX2INT(evalret) : 0;
  lis = CDR(lis);

  while(!NIL_P(lis))
    {
      evalret = eval(CAR(lis), env);
      acc -= FIXNUM_P(evalret) ? FIX2INT(evalret) : 0;
      lis = CDR(lis);
    }

  return INT2FIX(acc);
}

VALUE mul(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE evalret = Qnil;
  int acc = 1;

  while(!NIL_P(lis))
    {
      evalret = eval(CAR(lis), env);
      acc *= FIXNUM_P(evalret) ? FIX2INT(evalret) : 0;
      lis = CDR(lis);
    }

  return INT2FIX(acc);
}

VALUE divide(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE evalret = Qnil;
  int acc;

  evalret = eval(CAR(lis), env);
  acc = FIXNUM_P(evalret) ? FIX2INT(evalret) : 0;
  lis = CDR(lis);

  while(!NIL_P(lis))
    {
      evalret = eval(CAR(lis), env);
      acc /= FIXNUM_P(evalret) ? FIX2INT(evalret) : 0;
      lis = CDR(lis);
    }

  return INT2FIX(acc);
}

VALUE mod(VALUE args, VALUE env)
{
  VALUE lis = args;
  VALUE eval_lis = Qnil;
  while(!NIL_P(lis))
    {
      eval_lis = append(eval_lis, cons(eval(CAR(lis), env), Qnil));
      lis = CDR(lis);
    }
  int acc = FIX2INT(CAR(eval_lis));
  int acct = FIX2INT(CAR(CDR(eval_lis)));

  return INT2FIX(acc % acct);
}


VALUE  procedure_cons(VALUE  args, VALUE env)
{
  VALUE lis = args;
  VALUE eval_lis = Qnil;
  while(!NIL_P(lis))
    {
      eval_lis = append(eval_lis, cons(eval(CAR(lis), env), Qnil));
      lis = CDR(lis);
    }

  return cons(CAR(eval_lis), CAR(CDR(eval_lis)));
}

VALUE cond(VALUE args, VALUE env)
{
  VALUE lis = args;

  VALUE inlis;
  VALUE last_eval = Qnil;

  while(!NIL_P(lis))
    {
      inlis = CAR(lis);
      if(eval(CAR(inlis), env) == Qtrue)
	{
	  inlis = CDR(inlis);
	  while(!NIL_P(inlis))
	    {
	      last_eval = eval(CAR(inlis), env);
	      inlis = CDR(inlis);
	    }
	  break;
	}
      lis = CDR(lis);
    }

  return last_eval;
}

VALUE define(VALUE args, VALUE env)
{
  while(!NIL_P(CDR(env))) env = CDR(env);
  

  CDR(env) = cons(cons(CAR(args), CAR(CDR(args))), Qnil);

  return CAR(args);
}

VALUE assoc(VALUE e, VALUE lis)
{
  for(; !NIL_P(lis); lis = CDR(lis))
    {
      VALUE head = CAR(lis);
      if(strcmp(SYMBOL_NAME(e), SYMBOL_NAME(CAR(head))) == 0) return head;
    }
  return Qnil;
}

VALUE  pairlis(VALUE keys, VALUE values)
{
  VALUE lis = Qnil;

  while(!NIL_P(keys) && !NIL_P(values))
    {
      lis = append(lis, cons(cons(CAR(keys), CAR(values)), Qnil));
      keys = CDR(keys);
      values = CDR(values);
    }

    while((NIL_P(keys) && !NIL_P(values)) || (!NIL_P(keys) && NIL_P(values)))
    {
      fprintf(stderr, "argment error");
      exit(1);
    }

  return lis;
}
