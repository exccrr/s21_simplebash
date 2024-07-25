SHELL := /bin/bash
CC=gcc
CFLAGS= -Wall -Wextra -Werror 

all: main

main: s21_grep

s21_grep: s21_grep.o
	$(CC) $(CFLAGS) s21_grep.o -o s21_grep

s21_cat.o: s21_grep.c
	$(CC) $(CFLAGS) -c s21_grep.c

test:
	-diff <(./s21_grep -e sun ./tests/test1.txt) <(grep -e sun ./tests/test1.txt)
	-diff <(./s21_grep -i the ./tests/test2.txt) <(grep -i the ./tests/test2.txt)
	-diff <(./s21_grep -v 5 ./tests/test3.txt) <(grep -v 5 ./tests/test3.txt)
	-diff <(./s21_grep -c I ./tests/test4.txt) <(grep -c I ./tests/test4.txt)
	-diff <(./s21_grep -l a ./tests/*.txt) <(grep -l a ./tests/*.txt)
	-diff <(./s21_grep -n 1 ./tests/test6.txt) <(grep -n 1 ./tests/test6.txt)
	-diff <(./s21_grep -h a ./tests/*.txt) <(grep -h a ./tests/*.txt) 
	-diff <(./s21_grep -s a ./tests/test0.txt) <(grep -s a ./tests/test0.txt)
	-diff <(./s21_grep -o I ./tests/test10.txt) <(grep -o I ./tests/test10.txt)  

clean:
	rm -rf s21_grep *.o *.a *.out *.so

rebuild: clean all