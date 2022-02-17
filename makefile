CC = g++

LIB = lib
SRC = src
OBJ = obj
EXE = bin/binary

CFLAGS = -Wall -Wextra -I$(LIB) -std=c++17 -g

$(EXE): $(OBJ)/inverseIndex.o $(OBJ)/main.o $(OBJ)/linkedList.o
	$(CC) -o $(EXE) $(OBJ)/*.o

$(OBJ)/linkedList.o: $(LIB)/cell.hpp $(LIB)/linearList.hpp $(LIB)/linkedList.hpp $(SRC)/linkedList.cpp
	$(CC) $(CFLAGS) -c $(SRC)/linkedList.cpp -o $(OBJ)/linkedList.o

$(OBJ)/inverseIndex.o: $(LIB)/inverseIndex.hpp $(LIB)/linkedList.hpp $(SRC)/inverseIndex.cpp
	$(CC) $(CFLAGS) -c $(SRC)/inverseIndex.cpp -o $(OBJ)/inverseIndex.o

$(OBJ)/main.o: $(LIB)/inverseIndex.hpp $(SRC)/main.cpp
	$(CC) $(CFLAGS) -c $(SRC)/main.cpp -o $(OBJ)/main.o

clean:
	rm -rI $(OBJ)/* $(EXE)
