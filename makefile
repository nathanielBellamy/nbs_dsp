# Set the compiler
CC = gcc
CXX = g++

# Set flags
CFLAGS = -Wall
CXXFLAGS = -Wall
LDFLAGS = -lstdc++

# Set the names of your source, object, and executable files
C_SRC = ./src/c/main.c
CXX_SRC = ./src/cpp/foo.cpp
HEADER = ./src/cpp/foo.h
OBJ = $(C_SRC:.c=.o) $(CXX_SRC:.cpp=.o)
EXEC = ./bin/nbs_dsp

# All
all: $(EXEC)

# Executable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Object Files
%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
.PHONY: clean
clean:
	rm -f $(OBJ) $(EXEC)
