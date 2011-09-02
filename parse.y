%{
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "myLisp.h"

#define YYSTYPE LVALUE*

  typedef struct Str_tag {
    char *str;
    int len;
  } Str;

  FILE *yyin;
  int yylex(YYSTYPE*);
  int yyerror(char const*);
  void pushchar(Str* , int);
  int input();
  int unput(int);

%}

%token ATOM
%pure_parser
%%

toplevel
: s_expr {
  printf("=> ");
  printtree($1);
  printf("\nmyLisp> ");
 }
| toplevel s_expr {
  printf("=> ");
  printtree($2);
  printf("\nmyLisp> ");

}
;

s_expr
: ATOM
| '(' carpart ')' {$$ = $2;}
| '(' carpart '.' s_expr ')' {$$ = append($2, $4);}
| '(' ')' {$$ = make_atom(NIL, NULL);}
;

carpart
: s_expr {$$ = cons($1, make_atom(NIL, NULL));}
| carpart s_expr {$$ = append($1, cons($2, make_atom(NIL, NULL)));}
;

%%
int yylex (YYSTYPE *lvalp)
{
  int c;
  Str str = {NULL, 0};
  
  c = input();
  if (c == ';') while((c = input()) != '\n');

  unput(c);
  while(isspace(c = input()));
  if (c == EOF) return 0;
  else if (c == '(' || c == ')' || c == '.') return c;

  unput(c);
  while(isdigit(c = input()))
    {
      pushchar(&str, c);
    }

  if(isspace(c))
    {
      pushchar(&str, '\0');
      *lvalp = make_atom(INT, str.str);
      free(str.str);
      return ATOM;
    }
  
  unput(c);
  while(isgraph(c = input()) && !(c == '(' || c == ')' || c == ';'))
    {
      pushchar(&str, c);
    }

  pushchar(&str, '\0');
  *lvalp = make_atom(SYMBOL, str.str);

  unput(c);
  free(str.str);
  return ATOM;
}


int yyerror (char const *s)
{
  fprintf (stderr, "*** %s ***\n", s);
  return 0;
}
int input()
{
  return getc(yyin);
}

int unput(int c)
{
  return ungetc(c, yyin);
}

void pushchar(Str *str, int c)
{
  str->len++;
  /*printf("%d", str->len);*/
  str->str = (char*)realloc(str->str, sizeof(char) * str->len);
  str->str[str->len - 1] = (char)c;
}