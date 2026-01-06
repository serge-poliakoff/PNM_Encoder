#-- compilateur
CC = gcc

#-- standard
STD = -std=c17

#-- flags preprocessor
PFLAGS = -I./include/

#-- flags compilation : option(s) minimale(s)
CFLAGS = -Wall

#-- autre(s) option(s)
ifeq ($(GDB),1)
  CFLAGS += -g
else  
  CFLAGS += -O2
endif

#-- flags �dition de lien

#-- rep. sources
SRC = src/

OBJ = bits.o codage.o differential.o pnm.o dif.o
OBJS = $(patsubst %,bin/%,$(OBJ))

lib: $(OBJS)
	$(CC) -shared $^ -o lib/libbinpars.dll

main : bin/main.o bin/bits.o bin/codage.o
	$(CC)  $^ -o $@

#-- regle de compilation generique des objets
bin/%.o : $(SRC)%.c
	$(CC) $(STD) $(PFLAGS) $(CFLAGS) -c $< -o $@


#-- Nettoyage
clean : 
	rm -f *.o
cleanall : 
	rm -f *.o mainlist

#-- informations sur les param�tres de compilation
? :
	@echo "---------informations----------"
	@echo "  compilateur : $(CC)    "
	@echo "  standard    : $(STD)   "
	@echo "  PFLAGS      : $(PFLAGS)"
	@echo "  CFLAGS      : $(CFLAGS)"
	@echo "  LFLAGS      : $(LFLAGS)"
	@echo "-------------------------------"

.PHONY : clean cleanall ?
