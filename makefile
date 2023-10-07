# Set the compiler
CC = gcc
CXX = g++

# Set flags
CFLAGS = -Wall -I/usr/local/include
CXXFLAGS = -Wall
LDFLAGS = -lstdc++ -lsndfile -lportaudio -lfftw3 -lm -L/usr/local/lib

# Set the names of your source, object, and executable files
C_SRC = ./src/c/main.c ./src/c/pa.c
CXX_SRC = ./src/cpp/foo.cpp ./src/cpp/Cli.cpp
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
