PATH2LIB = ./parser/lib
PATHI = ./parser/include
LIBNAME = binpars
DEBUG = -g
CFLAGS = -Wall -std=c17

main: bin/main.o bin/getopt.o
	gcc $(CFLAGS) $(DEBUG) -L$(PATH2LIB) $< -l$(LIBNAME) -o main.exe

bin/%.o: src/%.c
	gcc $(CFLAGS) -I$(PATHI) -Wall -std=c17 -c $< -o $@ $(DEBUG)

clean:
	rm ./main.exe
	rm ./lib$(LIBNAME).dll
	rm -f *.o