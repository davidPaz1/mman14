CC = gcc
exeFlags = $(CC) -pedantic -ansi -Wall -g
compileFlags =  $(exeFlags) -c

run: main.o macroTable.o preprocessor.o util.o scan.o error.o
	$(exeFlags) main.o macroTable.o preprocessor.o util.o scan.o error.o -o run
main.o: main.c preprocessor.h macroTable.h 
	$(compileFlags) main.c
macroTable.o: macroTable.c macroTable.h error.h global.h
	$(compileFlags) macroTable.c
preprocessor.o: preprocessor.c preprocessor.h macroTable.h scan.h util.h error.h global.h
	$(compileFlags) preprocessor.c


util.o : util.c util.h global.h
	$(compileFlags) util.c
scan.o : scan.c scan.h global.h error.h	
	$(compileFlags) scan.c
error.o : error.c error.h global.h
	$(compileFlags) error.c


a:
	rm -rf *.o *.am *.ob *.ent *.ext *.exe run
	clear
	clear
c:
	rm -rf *.o *.am *.ob *.ent *.ext
pre:
	make
	make c
	clear 
	clear
r1:
	make pre
	./run test1
r2:
	make pre
	./run test2
rv:
	make pre
	valgrind ./run
vr:
	make rv
g: 
	make pre
	gdb ./run