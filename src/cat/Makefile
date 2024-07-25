SHELL := /bin/bash
CC=gcc
CFLAGS= -Wall -Wextra -Werror 

all: main

main: s21_cat

s21_cat: s21_cat.o CatFlags.o 
	$(CC) $(CFLAGS) s21_cat.o CatFlags.o -o s21_cat

s21_cat.o: s21_cat.c
	$(CC) $(CFLAGS) -c s21_cat.c

CatFlags.o: CatFlags.c
	$(CC) $(CFLAGS) -c CatFlags.c

test:
	-diff <(./s21_cat -b ./tests/test1.txt) <(cat -b ./tests/test1.txt)
	-diff <(./s21_cat -n ./tests/test3.txt) <(cat -n ./tests/test3.txt)
	-diff <(./s21_cat -s ./tests/test4.txt) <(cat -s ./tests/test4.txt)
	-diff <(./s21_cat -v ./tests/test6.txt) <(cat -v ./tests/test6.txt)
	-diff <(./s21_cat -e ./tests/test7.txt) <(cat -e ./tests/test7.txt)
	-diff <(./s21_cat -t ./tests/test8.txt) <(cat -t ./tests/test8.txt)

clean:
	rm -rf s21_cat *.o *.a *.out *.so

rebuild: clean all