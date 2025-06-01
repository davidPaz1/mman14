CC = gcc
exeFlags = $(CC) -pedantic -ansi -Wall -g
compileFlags =  $(exeFlags) -c

run: main.o base4.o util.o 
	$(exeFlags) main.o base4.o util.o -o run
base4.o: base4.c header.o
	$(compileFlags) base4.c
main.o: main.c util.o header.o macroTable.h
	$(compileFlags) main.c
util.o : util.c header.o
	$(compileFlags) util.c
header.o : util.c util.h global.h
	$(compileFlags) util.c
a:
	rm -rf *.o *.am *.ob *.ent *.ext *.exe run
c:
	rm -rf *.o *.am *.ob *.ent *.ext
o:
	rm -rf *.o
am:
	rm -rf *.am