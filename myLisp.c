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
