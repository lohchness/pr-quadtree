# define C compiler & flags
CC = gcc
CFLAGS = -Wall -g
# define libraries to be linked (for example -lm)
LIB = 

# # define sets of header source files and object files
SRC = driver.c dict.c data.c quadtree.c
# # OBJ is the same as SRC, just replace .c with .h
OBJ = $(SRC:.c=.o)

dict1: driver.o dict.o data.o quadtree.o
	$(CC) $(CFLAGS) -o dict1 $(OBJ) $(LIB)

dict2: driver.o dict.o data.o quadtree.o
	$(CC) $(CFLAGS) -o dict2 $(OBJ) $(LIB)

dict3: driver.o dict.o data.o quadtree.o
	$(CC) $(CFLAGS) -o dict3 $(OBJ) $(LIB)

dict4: driver.o dict.o data.o quadtree.o
	$(CC) $(CFLAGS) -o dict4 $(OBJ) $(LIB)

driver.o: driver.c

dict.o: dict.c dict.h

data.o: data.c data.h

quadtree.o: quadtree.c quadtree.h

clean:
	rm -f $(OBJ) dict1 dict2