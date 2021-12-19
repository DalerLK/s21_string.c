.PHONY: all clean test s21_string.a check gcov_report valgrind
CC=gcc
CC2=gcc
CFLAGS=-c
OS := $(shell uname -s)

ifeq ($(OS), Darwin)
    CC2 += -D OS_MAC
else
    CC2 += -D OS_LINUX
endif
default: all

all: clean s21_string.a test

test: s21_string.a
ifeq ($(OS), Darwin)
	$(CC) -o test string-test.c s21_string.a -lcheck
	./test
else
	$(CC) string-test.c s21_string.a -o test -lcheck -lsubunit -lrt -lpthread -lm
	./test
endif
s21_string.a: s21_string.o s21_sscanf.o s21_sprintf.o
	ar rcs s21_string.a s21_string.o s21_sscanf.o s21_sprintf.o
	ranlib $@
	cp $@ lib$@
    
s21_string.o: s21_string.c
	$(CC2) -c -o s21_string.o s21_string.c
s21_sscanf.o: s21_sscanf.c
	$(CC2) -c -o s21_sscanf.o s21_sscanf.c
s21_sprintf.o: s21_sprintf.c
	$(CC2) -c -o s21_sprintf.o s21_sprintf.c

clean:
	rm -rf test *.a *.o *.out *.cfg fizz *.gc* *.info report

rebuild: clean all

check:
	cppcheck *.h *.c
	cp ../materials/linters/CPPLINT.cfg CPPLINT.cfg
	python3 ../materials/linters/cpplint.py --extension=c *.c *.h
	CK_FORK=no leaks --atExit -- ./test

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=RESULT_VALGRIND.txt ./test

gcov_report: 
ifeq ($(OS), Darwin)
	$(CC2) --coverage $(LDFLAGS) string-test.c s21_string.c s21_sprintf.c s21_sscanf.c -o test -lcheck
else
	$(CC2) --coverage $(LDFLAGS) string-test.c s21_string.c s21_sprintf.c s21_sscanf.c -o test -lcheck -lsubunit -lrt -lpthread -lm
endif
	./test
	lcov -t "test" -o test.info -c -d ./
	genhtml -o report test.info


