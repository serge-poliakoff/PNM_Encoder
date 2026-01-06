PATH2LIB = ./parser/lib
PATHI = ./parser/include
LIBNAME = binpars
DEBUG = -g

main: bin/main.o
	gcc $(DEBUG) -L$(PATH2LIB) $< -l$(LIBNAME) -o main.exe


bin/%.o: src/%.c
	gcc -I$(PATHI) -Wall -std=c17 -c $< -o $@ $(DEBUG)

clean:
	rm ./main.exe
	rm ./lib$(LIBNAME).dll
	rm -f *.o