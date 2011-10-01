YACC = bison
CC = gcc
CFLAGS = -g -O0 -Wall -Wextra -Wformat=2 -std=c89

a.out: parse.y myLisp.c myLisp.h gc.c eval.c
	$(YACC) parse.y
	$(CC) $(CFLAGS) myLisp.c parse.tab.c gc.c eval.c 

.PHONY: run debug clean	
run: 
	./a.out

debug:
	gdb ./a.out

clean:
	rm -f ./a.out parse.tab.c