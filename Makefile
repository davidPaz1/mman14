run: main.o base4.o
	gcc -pedantic -ansi -Wall main.o base4.o -o run
base4.o: base4.c define.h
	gcc -c -pedantic -ansi -Wall base4.c
main.o: main.c define.h
	gcc -c -pedantic -ansi -Wall main.c