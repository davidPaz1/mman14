run: main.o base4.o
	gcc -pedantic -ansi -Wall main.o base4.o -o run
base4.o: base4.c global.h
	gcc -c -pedantic -ansi -Wall base4.c
main.o: main.c global.h
	gcc -c -pedantic -ansi -Wall main.c

c:
	rm -rf *.o *.am *.ob *.ent *.ext 
o:
	rm -rf *.o
a:
	rm -rf *.o *.am *.ob *.ent *.ext *.exe run