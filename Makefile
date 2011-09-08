YACC = bison
CC = gcc
CFLAGS = -g -O0 -Wall -std=c89

a.out: parse.y myLisp.c
	$(YACC) parse.y
	$(CC) $(CFLAGS) myLisp.c parse.tab.c 

.PHONY: run debug clean	
run: 
	./a.out

debug:
	gdb ./a.out

clean:
	rm -f ./a.out parse.tab.c