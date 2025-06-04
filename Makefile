CC = gcc
exeFlags = $(CC) -pedantic -ansi -Wall -g
compileFlags =  $(exeFlags) -c

run: main.o macroTable.o preprocessor.o util.o scan.o
	$(exeFlags) main.o macroTable.o preprocessor.o util.o scan.o -o run
main.o: main.c preprocessor.h macroTable.h
	$(compileFlags) main.c
macroTable.o: macroTable.c macroTable.h global.h util.h
	$(compileFlags) macroTable.c
preprocessor.o: preprocessor.c preprocessor.h global.h scan.h macroTable.h
	$(compileFlags) preprocessor.c


util.o : util.c util.h global.h
	$(compileFlags) util.c
scan.o : scan.c scan.h global.h
	$(compileFlags) scan.c

a:
	rm -rf *.o *.am *.ob *.ent *.ext *.exe run
	clear
	clear
c:
	rm -rf *.o *.am *.ob *.ent *.ext
o:
	rm -rf *.o
am:
	rm -rf *.am
r:
	make o
	clear 
	clear
	./run
rv:
	valgrind ./run