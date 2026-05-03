CC     = gcc
CFLAGS = -Wall -g

all: json2xml

json2xml: parser.tab.o lex.yy.o ast.o main.o
	$(CC) $(CFLAGS) -o json2xml parser.tab.o lex.yy.o ast.o main.o

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: scanner.l parser.tab.h
	flex scanner.l

parser.tab.o: parser.tab.c ast.h
	$(CC) $(CFLAGS) -c parser.tab.c

lex.yy.o: lex.yy.c ast.h
	$(CC) $(CFLAGS) -c lex.yy.c

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

main.o: main.c ast.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o lex.yy.c parser.tab.c parser.tab.h json2xml