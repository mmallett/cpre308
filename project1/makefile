CC=gcc
CFLAGS=-c -Wall

all: parser_test

parser_test: parser.o printer.o parser_test.o shell.h
	$(CC) parser_test.o parser.o printer.o -o parser_test

parser_test.o: parser_test.c shell.h
	$(CC) $(CFLAGS) parser_test.c

parser.o: parser.c shell.h
	$(CC) $(CFLAGS) parser.c

printer.o: printer.c shell.h
	$(CC) $(CFLAGS) printer.c

clean:
	rm -rf *o parser_test
