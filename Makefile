COMPILER=gcc
OUTPUT_FILE=censo
FLAGS=-Wall -pedantic -std=c99 -fsanitize=address

all: back.o
		 $(COMPILER) -o $(OUTPUT_FILE) front.c back.o $(FLAGS)

back.o: back.c 
				$(COMPILER) -c back.c 

clean:
			rm -rf $(OUTPUT_FILE) *.o *.h.gch
