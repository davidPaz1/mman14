CC = gcc
exeFlags = $(CC) -pedantic -ansi -Wall -g
compileFlags =  $(exeFlags) -c

run: main.o macroTable.o preprocessor.o firstPass.o util.o scan.o error.o
	$(exeFlags) main.o macroTable.o preprocessor.o firstPass.o util.o scan.o error.o -o run
main.o: main.c preprocessor.h firstPass.h error.h global.h
	$(compileFlags) main.c
macroTable.o: macroTable.c macroTable.h error.h global.h
	$(compileFlags) macroTable.c
preprocessor.o: preprocessor.c preprocessor.h macroTable.h scan.h util.h error.h global.h
	$(compileFlags) preprocessor.c
firstPass.o: firstPass.c firstPass.h scan.h util.h error.h global.h
	$(compileFlags) firstPass.c


util.o : util.c util.h global.h
	$(compileFlags) util.c
scan.o : scan.c scan.h global.h error.h	
	$(compileFlags) scan.c
error.o : error.c error.h global.h
	$(compileFlags) error.c


a:
	rm -rf *.o *.am *.ob *.ent *.ext *.exe run
	clear
c:
	rm -rf *.o *.am *.ob *.ent *.ext
o:
	rm -rf *.o
pre:
	make
	make c
	clear 
r:
	make pre
	clear
	./run test1
r2:
	make pre
	clear
	./run test2
rv:
	make pre
	clear
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./run
vr:
	make rv
g: 
	make pre
	gdb ./run
e:
	make a
	clear
	make -s
	rm -rf *.o *.am *.ob *.ent *.ext *.exe run