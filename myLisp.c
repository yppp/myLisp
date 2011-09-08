#include "myLisp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LVALUE* make_atom(Type type, char *str)
{
  LVALUE *r = (LVALUE*)malloc (sizeof(LVALUE));
  r->type = type;

  char *cp = NULL;

  if(str != NULL)
    {
      cp = (char*)malloc (sizeof(char) * (strlen(str) + 1));
      strcpy(cp, str);
    }

  switch(type)
    {
    case INT:
      r->u.integer = atoi(cp);
      free(cp);
      break;
    case SYMBOL:
      r->u.symbol = cp;
      break;
    case NIL:
      break;
    case CELL:
      break;
    }
  return r;
}

LVALUE* cons(LVALUE *car, LVALUE *cdr)
{
  LVALUE *cell = (LVALUE*)malloc(sizeof(LVALUE));
  cell->type = CELL;
  CAR(cell) = car;
  CDR(cell) = cdr;
  return cell;
}

LVALUE* append(LVALUE *lis, LVALUE *a)
{
  LVALUE *tmp;
  if (NIL_P(lis)) return a;

  for(tmp = lis; !(NIL_P(CDR(tmp))); tmp = CDR(tmp));

  CDR(tmp) = a;
  return lis;
}

int main(void)
{
  extern int yyparse();
  
  prompt();
  while(yyparse());

  return 0;
}

void print_tree(LVALUE *tree)
{
  if(NIL_P(tree)) printf("()");
  else if(INT_P(tree)) printf("%d", tree->u.integer);
  else if(SYMBOL_P(tree)) printf("%s", tree->u.symbol);
  else if (PAIR_P(tree))
    {
      printf("(");
      while(1)
	{
	  print_tree(CAR(tree));
	  tree = CDR(tree);
	  if(NIL_P(tree)) break;
	  
	  if(INT_P(tree) || SYMBOL_P(tree))
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

LVALUE* eval(LVALUE *tree, LVALUE *env)
{
  LVALUE *v;
  if (NIL_P(tree) || INT_P(tree)) return tree;
  else if (PAIR_P(tree)) 
    {

      if(strcmp(SYMBOL_NAME(CAR(tree)), "lambda") == 0) return tree;
      /*
      if(strcmp(SYMBOL_NAME(CAR(tree)), "car") == 0) return procedure_car(tree, env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "cdr") == 0) return procedure_cdr(tree, env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "cons") == 0) return procedure_cons(tree, env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "eq") == 0) return eq(tree, env);
      if(strcmp(SYMBOL_NAME(CAR(tree)), "atom") == 0) return atom(tree, env);
      */

      return apply(CAR(tree), CDR(tree), env);
    }
  else if (SYMBOL_P(tree))
    {
      v = assoc(tree, env);
      if (!NIL_P(v)) return CDR(v);
      fprintf(stderr, "Undefined symbol '%s'", SYMBOL_NAME(tree));
      return make_atom(NIL, NULL);
    }
  fprintf(stderr, "Undefined type '%d'", tree->type);
  exit(1);
  return (LVALUE*)0;
}
/*((lambda (x) x) 1)*/
LVALUE* apply(LVALUE *func, LVALUE *args, LVALUE* env)
{
  LVALUE *fbody = eval(func, env);
  LVALUE *ftype = CAR(fbody);

  if(NIL_P(fbody) || (SYMBOL_P(ftype) && strcmp(SYMBOL_NAME(ftype), "lambda") != 0))
    {
      fprintf(stderr, "invalid application ");
      print_tree(cons(func, args));
      return make_atom(NIL, NULL); 
    }


  LVALUE *params = CAR(CDR(fbody));
  LVALUE *e = CAR(CDR(CDR(fbody)));
  
  LVALUE *lis = args;
  LVALUE *eval_lis = make_atom(NIL, NULL);
  while(!NIL_P(lis))
    {
      eval_lis = append(eval_lis, cons(eval(CAR(lis), env), make_atom(NIL, NULL)));
      lis = CDR(lis);
    }
  
  return eval(e, append(pairlis(params, eval_lis), env));
}

LVALUE* assoc(LVALUE *e, LVALUE *lis)
{
  for(; !NIL_P(lis); lis = CDR(lis))
    {
      LVALUE *head = CAR(lis);
      if(strcmp(SYMBOL_NAME(e), SYMBOL_NAME(CAR(head))) == 0) return head;
    }
  return make_atom(NIL, NULL);
}

LVALUE* pairlis(LVALUE *keys, LVALUE *values)
{
  LVALUE *lis = make_atom(NIL, NULL);

  while(!NIL_P(keys) && !NIL_P(values))
    {
      lis = append(lis, cons(cons(CAR(keys), CAR(values)), make_atom(NIL, NULL)));
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
