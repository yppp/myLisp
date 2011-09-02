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

LVALUE* car(LVALUE *obj)
{
  return obj->u.cell.car;
}

LVALUE* cdr(LVALUE *obj)
{
  return obj->u.cell.cdr;
}


LVALUE* cons(LVALUE *car, LVALUE *cdr)
{
  LVALUE *cell = (LVALUE*)malloc(sizeof(LVALUE));
  cell->type = CELL;
  cell->u.cell.car = car;
  cell->u.cell.cdr = cdr;
  return cell;
}

LVALUE* append(LVALUE *lis, LVALUE *a)
{
  LVALUE *tmp;
  if (lis->type == NIL) return a;

  for(tmp = lis; tmp->u.cell.cdr->type != NIL; tmp = cdr(tmp));

  tmp->u.cell.cdr = a;
  return lis;
}

int main(void)
{
  extern int yyparse();
  extern FILE *yyin;

  yyin = stdin;
  
  printf("myLisp> ");
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
      if(tree->type == NIL) printf("()");
      else if(tree->type == INT) printf("%d", tree->u.integer);
	else if(tree->type == SYMBOL) printf("%s", tree->u.symbol);
      return;
    }

  printf(" (");
  printtree(tree->u.cell.car);
  printf(" . ");
  printtree(tree->u.cell.cdr);
  printf(") ");
}
