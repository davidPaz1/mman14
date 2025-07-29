CC = gcc
exeFlags = $(CC) -pedantic -ansi -Wall -g
compileFlags =  $(exeFlags) -c

run: main.o tables.o preprocessor.o firstPass.o secondPass.o util.o lexer.o error.o
	$(exeFlags) main.o tables.o preprocessor.o firstPass.o secondPass.o util.o lexer.o error.o -o run
main.o: main.c preprocessor.h firstPass.h secondPass.h error.h global.h lexer.h tables.h util.h
	$(compileFlags) main.c
tables.o: tables.c tables.h global.h error.h lexer.h util.h
	$(compileFlags) tables.c
preprocessor.o: preprocessor.c preprocessor.h global.h error.h lexer.h util.h tables.h
	$(compileFlags) preprocessor.c
firstPass.o: firstPass.c firstPass.h global.h error.h lexer.h util.h tables.h 
	$(compileFlags) firstPass.c
secondPass.o: secondPass.c secondPass.h global.h error.h lexer.h util.h tables.h
	$(compileFlags) secondPass.c


util.o : util.c util.h global.h
	$(compileFlags) util.c
lexer.o : lexer.c lexer.h global.h error.h  util.h tables.h
	$(compileFlags) lexer.c
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
	make
	rm -rf *.am *.ob *.ent *.ext
	clear
	./run test1
ra:
	make
	rm -rf *.am *.ob *.ent *.ext *.o
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
s:
	make e