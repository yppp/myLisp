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
  else if(FIXNUM_P(tree)) printf("%ld", FIX2INT(tree));
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
