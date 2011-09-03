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
  while(yyparse())
    {
      exit(1);
    }
  return 0;
}

void printtree(LVALUE *tree)
{
  if(tree->type != CELL)
    {
      if(NIL_P(tree)) printf("()");
      else if(INT_P(tree)) printf("%d", tree->u.integer);
      else if(SYMBOL_P(tree)) printf("%s", tree->u.symbol);
      return;
    }

  printf(" (");
  printtree(CAR(tree));
  printf(" . ");
  printtree(CDR(tree));
  printf(") ");
}


void prompt()
{
  printf("myLisp> ");
}
