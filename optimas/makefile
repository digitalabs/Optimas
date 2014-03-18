#----------
# MAKEFILE
#----------

PATH_OBJ    = obj/
PATH_SCR    = src/
PATH_HEADER = headers/
PATH_BIN    = bin/

PROG        = optimas
CC          = g++
LINKER      = g++
FLAGS       = -O3 -Wall -lm -Wextra -Wwrite-strings

OBJ = $(PATH_OBJ)main.o $(PATH_OBJ)utils.o $(PATH_OBJ)params.o $(PATH_OBJ)markers.o $(PATH_OBJ)gametes.o $(PATH_OBJ)haplotypes.o $(PATH_OBJ)individual.o $(PATH_OBJ)results.o


#---------------------------
# GENERAL COMPILATION RULES
#---------------------------

$(PATH_OBJ)%.o: $(PATH_SCR)%.cpp $(PATH_HEADER)%.h
	$(CC) -c $(FLAGS) $< -o $@
	
$(PATH_HEADER)%.h:
	

#-----------------
# EXECUTABLE RULE
#-----------------

$(PATH_BIN)$(PROG): $(OBJ)
	$(LINKER) $(FLAGS) $^ -o $@


#-------------
# DEPENDENCES
#-------------

$(PATH_OBJ)main.o: $(PATH_HEADER)main.h $(PATH_HEADER)utils.h $(PATH_HEADER)params.h $(PATH_HEADER)markers.h $(PATH_HEADER)individual.h $(PATH_HEADER)gametes.h
$(PATH_OBJ)utils.o: $(PATH_HEADER)utils.h
$(PATH_OBJ)params.o: $(PATH_HEADER)utils.h $(PATH_HEADER)params.h
$(PATH_OBJ)markers.o: $(PATH_HEADER)utils.h $(PATH_HEADER)params.h $(PATH_HEADER)markers.h
$(PATH_OBJ)gametes.o: $(PATH_HEADER)utils.h $(PATH_HEADER)params.h $(PATH_HEADER)markers.h $(PATH_HEADER)gametes.h
$(PATH_OBJ)haplotypes.o: $(PATH_HEADER)utils.h $(PATH_HEADER)params.h $(PATH_HEADER)markers.h $(PATH_HEADER)gametes.h $(PATH_HEADER)haplotypes.h
$(PATH_OBJ)individual.o: $(PATH_HEADER)utils.h $(PATH_HEADER)params.h $(PATH_HEADER)markers.h $(PATH_HEADER)gametes.h $(PATH_HEADER)haplotypes.h $(PATH_HEADER)individual.h
$(PATH_OBJ)results.o: $(PATH_HEADER)utils.h $(PATH_HEADER)params.h $(PATH_HEADER)markers.h $(PATH_HEADER)individual.h $(PATH_HEADER)results.h


all:
	g++ -O3 -Wall -Wextra -Wwrite-strings -c -o obj/main.o src/main.cpp
	g++ -O3 -Wall -Wextra -Wwrite-strings -c -o obj/utils.o src/utils.cpp
	g++ -O3 -Wall -Wextra -Wwrite-strings -c -o obj/params.o src/params.cpp
	g++ -O3 -Wall -Wextra -Wwrite-strings -c -o obj/markers.o src/markers.cpp
	g++ -O3 -Wall -Wextra -Wwrite-strings -c -o obj/gametes.o src/gametes.cpp
	g++ -O3 -Wall -Wextra -Wwrite-strings -c -o obj/haplotypes.o src/haplotypes.cpp
	g++ -O3 -Wall -Wextra -Wwrite-strings -c -o obj/individual.o src/individual.cpp
	g++ -O3 -Wall -Wextra -Wwrite-strings -c -o obj/results.o src/results.cpp
	g++ -O3 -Wall -lm -Wextra -Wwrite-strings -o bin/optimas obj/main.o obj/utils.o obj/params.o obj/markers.o obj/gametes.o obj/haplotypes.o obj/individual.o obj/results.o

clean:
	rm $(PATH_OBJ)*.o
	rm $(PATH_BIN)$(PROG)
	rm $(PATH_HEADER)*.h~
	rm $(PATH_SCR)*.cpp~
