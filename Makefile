CC = g++
LIBS_DIR = lib/
INCLUDE_DIR = includes/

CLFAGS = -Wall -std=c++20 -I$(INCLUDE_DIR) -L$(LIBS_DIR)  

LIBS= -lraylib -lopengl32 -lgdi32 -lwinmm

EXECUTABLE = main

SRC = ${wildcard *.cpp}

all: $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE).exe



$(EXECUTABLE): $(SRC)
	$(CC) $^ $(CLFAGS) $(LIBS) -o $(EXECUTABLE).exe 

$@: $@.cpp
	$(CC) $^ $(CLFAGS) -o $@.exe 
