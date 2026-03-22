CC = g++
LIBS_DIR = lib/
INCLUDE_DIR = includes/

CLFAGS = -Wall -std=c++20 -I$(INCLUDE_DIR) -L$(LIBS_DIR) -ggdb 
ifeq ($(OS),Windows_NT)
    LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm
    EXEC_EXT = .exe
else
    LDFLAGS =  -lraylib -lX11
    EXEC_EXT =
endif

EXECUTABLE = main

SRC = ${wildcard *.cpp}

all: $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)$(EXEC_EXT)



$(EXECUTABLE): $(SRC)
	$(CC) $^ $(CLFAGS) $(LDFLAGS) -o $(EXECUTABLE)$(EXEC_EXT) 

$@: $@.cpp
	$(CC) $^ $(CLFAGS) -o $@$(EXEC_EXT) 
