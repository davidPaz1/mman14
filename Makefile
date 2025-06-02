CC = gcc
exeFlags = $(CC) -pedantic -ansi -Wall -g
compileFlags =  $(exeFlags) -c

run: main.o util.o macroTable.o
	$(exeFlags) main.o util.o macroTable.o -o run
main.o: main.c util.o macroTable.h
	$(compileFlags) main.c
macroTable.o: macroTable.c macroTable.h global.h util.h
	$(compileFlags) macroTable.c
util.o : util.c util.h global.h
	$(compileFlags) util.c


a:
	rm -rf *.o *.am *.ob *.ent *.ext *.exe run
c:
	rm -rf *.o *.am *.ob *.ent *.ext
o:
	rm -rf *.o
am:
	rm -rf *.am