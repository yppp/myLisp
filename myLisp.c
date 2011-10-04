#include "myLisp.h"

VALUE cons(VALUE car, VALUE cdr)
{
  LVALUE *cell = make_obj(Qnil);
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
  int i;
  extern int yyparse();
  topenv = Qnil;
  freelist = NULL;

  envstack.envs = NULL;
  envstack.len = 0;

  static Subr subrs[] = {procedure_car, procedure_cdr, procedure_cons, eq, atom, add, sub, mul, divide, mod, cond, lambda, quote, define, define_macro, set};
  
  static const char* subrnames[] = {"car", "cdr", "cons", "=", "atom", "+", "-", "*", "/", "%", "cond", "lambda", "quote", "define", "define-macro", "set!"};


  for(i = 0; i < NELEMS(subrs); i++)
    {
      defsubr(subrnames[i], subrs[i]);
    }

  prompt();
  while(yyparse());

  return 0;
}

void print_tree(VALUE tree)
{
  if(NIL_P(tree)) printf("()");
  else if(FALSE_P(tree)) printf("#f");
  else if(TRUE_P(tree)) printf("#t");
  else if(FIXNUM_P(tree)) printf("%ld", FIX2INT(tree));
  else if(SYMBOL_P(tree)) printf("%s", SYMBOL_NAME(tree));
  else if(CLOSURE_P(tree)) printf("#<closure>");
  else if(MACRO_P(tree)) printf("#<macro>");
  else if(NATIVE_PROCEDURE_P(tree)) printf("#<subr>");
  else if (PAIR_P(tree))
    {
      printf("(");
      while(1)
	{
	  print_tree(CAR(tree));
	  tree = CDR(tree);
	  if(NIL_P(tree)) break;
	  
	  if(DIRECTVAL_P(tree) || SYMBOL_P(tree) || CLOSURE_P(tree) || MACRO_P(tree) || NATIVE_PROCEDURE_P(tree))
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


void defsubr(const char *name, Subr pf)
{
  VALUE v = make_symbol(name, Qnil);

  VALUE f = (VALUE)make_obj(Qnil);
  ((LVALUE*)f)->u.basic.type = NATIVE_PROCEDURE;
  ((LVALUE*)f)->u.native.proc = pf;

  if(NIL_P(topenv))
    {
      topenv = cons(cons(v, f), Qnil);
    }
  else
    {
      topenv = append(cons(cons(v, f), Qnil), topenv);
    }
}

void envpush(VALUE env)
{
  envstack.envs = realloc(envstack.envs, sizeof(VALUE*) * (envstack.len + 1));
  envstack.envs[envstack.len] = env;
  envstack.len++;

  return;
}

void envpop()
{
  envstack.envs = realloc(envstack.envs, sizeof(VALUE*) * (envstack.len - 1));
  envstack.len--;

  return;
}
