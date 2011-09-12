#include "myLisp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LVALUE* make_obj()
{
  int i,j;
  LVALUE *r;
  if(freelist == NULL)
    {
      heap.len += HEAP_GROW;
      if((heap.slots = (Heapslot*)realloc(heap.slots, sizeof(Heapslot) * heap.len)) == NULL)
	{
	  fprintf(stderr, "memory allocate error\n");
	  exit(1);
	}
      for(i = HEAP_GROW; i > 0; i--)
	{
	  for(j = 0; j < SLOT_SIZE; j++)
	    {
	      heap.slots[heap.len - i].values[j].u.basic.gc_mark = 1;
	    }
	}
      gc();
    }

  r = freelist;
  freelist = freelist->u.free.next;

  return r;
}

void gc()
{
  int i,j;
  LVALUE *slot;
  LVALUE *lastfree;
  LVALUE *tmpenv;
  for(tmpenv = (LVALUE*)topenv; !NIL_P(tmpenv); tmpenv = (LVALUE*)CDR(tmpenv))
    {
      if(!DIRECTVAL_P(CDR(CAR(tmpenv)))) recursive_mark((LVALUE*)CDR(CAR(tmpenv)));
    }


  if(freelist == NULL)
    {
      lastfree = NULL;
    }
  else
    {
      for(lastfree = freelist; lastfree->u.free.next != NULL; lastfree = lastfree->u.free.next);
    }

  for(i = 0; i < heap.len; i++)
    {
      for(j = 0; j < SLOT_SIZE; j++)
	{
	  slot = (heap.slots[i].values) + j;
	  if(slot->u.basic.gc_mark == 1)
	    {
	      if(SYMBOL_P(slot)) free(slot->u.symbol);
	      slot->u.basic.gc_mark = 0;
	      slot->u.basic.type = FREE;
	      if(lastfree == NULL)
		{
		  freelist = slot;
		  lastfree = slot;
		}
	      else
		{
		  lastfree->u.free.next = slot;
		  lastfree = slot;
		}
	    }
	}
    }
}

void recursive_mark(LVALUE *v)
{
  if(DIRECTVAL_P(v) || v->u.basic.gc_mark == 1) return;

  v->u.basic.gc_mark = 1;

  if(SYMBOL_P(v) || CLOSURE_P(v) || MACRO_P(v)) return;
  else if(PAIR_P(v))
    {
      recursive_mark((LVALUE*)CAR(v));
      recursive_mark((LVALUE*)CDR(v));
    }
  
  fprintf(stderr, "recursive mark error\n");
  exit(1);
}

VALUE make_symbol(char *str)
{
  LVALUE *r = make_obj();
  r->u.basic.type = SYMBOL;

  char *cp = NULL;

  if(str != NULL)
    {
      cp = (char*)malloc (sizeof(char) * (strlen(str) + 1));
      strcpy(cp, str);
    }
  r->u.symbol = cp;

  return (VALUE)r;
}

VALUE cons(VALUE car, VALUE cdr)
{
  LVALUE *cell = make_obj();
  cell->u.basic.type = CELL;
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
  topenv = Qnil;
  freelist = NULL;
  
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
  else if(CLOSURE_P(tree)) printf("#<closure>");
  else if(MACRO_P(tree)) printf("#<macro>");
  else if (PAIR_P(tree))
    {
      printf("(");
      while(1)
	{
	  print_tree(CAR(tree));
	  tree = CDR(tree);
	  if(NIL_P(tree)) break;
	  
	  if(DIRECTVAL_P(tree) || SYMBOL_P(tree) || CLOSURE_P(tree) || MACRO_P(tree))
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

VALUE eval(VALUE tree, VALUE env)
{
  VALUE v;
  if (DIRECTVAL_P(tree)) return tree;
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
      if(strcmp(SYMBOL_NAME(CAR(tree)), "lambda") == 0) return lambda(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "quote") == 0) return quote(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "define") == 0) return define(CDR(tree), env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "define-macro") == 0) return define_macro(CDR(tree), env);
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
  
  if(!DIRECTVAL_P(fbody))
    {
      if(CLOSURE_P(fbody))
	{

	  VALUE params = GETPARAMS(fbody);
	  VALUE e = GETE(fbody);

  
	  VALUE lis = args;
	  VALUE eval_lis = Qnil;
	  while(!NIL_P(lis))
	    {
	      eval_lis = append(eval_lis, cons(eval(CAR(lis), env), Qnil));
	      lis = CDR(lis);
	    }
  
	  return eval(e, append(pairlis(params, eval_lis), env));
	}
      else if(MACRO_P(fbody))
	{
	  VALUE params = GETPARAMS(fbody);
	  VALUE e = GETE(fbody);
	  VALUE r = eval(e, append(pairlis(params, args), env));

	  return eval(r, env);
	}
    }


    fprintf(stderr, "invalid application ");
    print_tree(cons(func, args));
    return Qnil; 
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
  VALUE ltop = topenv;
  if (NIL_P(ltop)) 
    {
      topenv = cons(cons(CAR(args), eval(CAR(CDR(args)), env)), Qnil);
    }
  else
    {
      topenv = append(cons(cons(CAR(args), eval(CAR(CDR(args)), env)), Qnil), topenv);
    }
  return CAR(args);
}

VALUE define_macro(VALUE args, VALUE env)
{
  LVALUE *macro = make_obj();
  macro->u.basic.type = MACRO;
  GETPARAMS(macro) = CDR(CAR(args));
  GETE(macro) = CAR(CDR(args));

  VALUE ltop = topenv;
  if (NIL_P(ltop)) 
    {
      topenv = cons(cons(CAR(CAR(args)), (VALUE)macro), Qnil);
    }
  else
    {
      while(!NIL_P(CDR(ltop))) ltop = CDR(ltop);

      topenv = append(cons(cons(CAR(CAR(args)), (VALUE)macro), Qnil), topenv);
    }
  print_tree(env);
  return CAR(CAR(args));
}

VALUE lambda(VALUE args, VALUE env)
{
  LVALUE *lambda = make_obj();
  lambda->u.basic.type = CLOSURE;
  GETPARAMS(lambda) = CAR(args);
  GETE(lambda) = CAR(CDR(args));

  return (VALUE)lambda;
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

VALUE pairlis(VALUE keys, VALUE values)
{
  VALUE lis = Qnil;

  while(!NIL_P(keys) && !NIL_P(values))
    {
      if(SYMBOL_P(keys))
	{
	  lis = append(lis, cons(cons(keys, values), Qnil));
	  break;
	}
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
