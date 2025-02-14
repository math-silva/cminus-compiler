CC = gcc
LEX = flex
YACC = bison
CFLAGS = -g

SRC = src/
OBJS = $(SRC)lex.yy.o $(SRC)parser.tab.o $(SRC)utils.o $(SRC)symtab.o $(SRC)analyze.o $(SRC)main.o

all: cminus

$(SRC)lex.yy.c: $(SRC)scanner.l
	$(LEX) -o $(SRC)lex.yy.c $(SRC)scanner.l

$(SRC)parser.tab.c $(SRC)parser.tab.h: $(SRC)parser.y
	$(YACC) -v -d -o $(SRC)parser.tab.c $(SRC)parser.y

$(SRC)lex.yy.o: $(SRC)lex.yy.c $(SRC)parser.tab.h
	$(CC) $(CFLAGS) -c $(SRC)lex.yy.c -o $(SRC)lex.yy.o

$(SRC)parser.tab.o: $(SRC)parser.tab.c
	$(CC) $(CFLAGS) -c $(SRC)parser.tab.c -o $(SRC)parser.tab.o

$(SRC)utils.o: $(SRC)utils.c $(SRC)utils.h
	$(CC) $(CFLAGS) -c $(SRC)utils.c -o $(SRC)utils.o

$(SRC)symtab.o: $(SRC)symtab.c $(SRC)symtab.h
	$(CC) $(CFLAGS) -c $(SRC)symtab.c -o $(SRC)symtab.o

$(SRC)analyze.o: $(SRC)analyze.c $(SRC)analyze.h $(SRC)symtab.h $(SRC)globals.h
	$(CC) $(CFLAGS) -c $(SRC)analyze.c -o $(SRC)analyze.o

$(SRC)main.o: $(SRC)main.c $(SRC)globals.h $(SRC)utils.h $(SRC)symtab.h $(SRC)analyze.h
	$(CC) $(CFLAGS) -c $(SRC)main.c -o $(SRC)main.o

cminus: $(OBJS)
	$(CC) $(CFLAGS) -o cminus $(OBJS) -lfl

clean:
	rm -f $(SRC)*.o $(SRC)lex.yy.c $(SRC)parser.tab.c $(SRC)parser.tab.h $(SRC)parser.output cminus
