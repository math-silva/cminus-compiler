CC = gcc
LEX = flex
YACC = bison
CFLAGS = -g

OBJS = lex.yy.o parser.tab.o utils.o symtab.o analyze.o main.o

all: cminus

lex.yy.c: scanner.l
	$(LEX) scanner.l

parser.tab.c parser.tab.h: parser.y
	$(YACC) -v -d parser.y

lex.yy.o: lex.yy.c parser.tab.h
	$(CC) $(CFLAGS) -c lex.yy.c

parser.tab.o: parser.tab.c
	$(CC) $(CFLAGS) -c parser.tab.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

symtab.o: symtab.c symtab.h
	$(CC) $(CFLAGS) -c symtab.c

analyze.o: analyze.c analyze.h symtab.h globals.h
	$(CC) $(CFLAGS) -c analyze.c

main.o: main.c globals.h utils.h symtab.h analyze.h
	$(CC) $(CFLAGS) -c main.c

cminus: $(OBJS)
	$(CC) $(CFLAGS) -o cminus $(OBJS) -lfl

clean:
	rm -f *.o lex.yy.c parser.tab.c parser.tab.h parser.output cminus
