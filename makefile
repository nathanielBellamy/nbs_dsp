# Set the compiler
CC = gcc
CXX = g++ -std=c++20

# Set flags
CFLAGS = -Wall -I/usr/local/include
CXXFLAGS = -Wall
LDFLAGS = -lpthread -lstdc++ -lsndfile -lportaudio -lfftw3f -lm -L/usr/local/lib

# Set the names of your source, object, and executable files
C_SRC = ./src/c/main.c ./src/c/audio.c ./src/c/visual.c
HEADER_C = ./src/c/audio.h ./src/c/visual.h

CXX_SRC = ./src/cpp/extern_c.cpp ./src/cpp/Cli.cpp ./src/cpp/PolynomialConsoleGraph/src/Compute.cpp ./src/cpp/PolynomialConsoleGraph/src/Draw.cpp ./src/cpp/PolynomialConsoleGraph/src/Settings.cpp
HEADER_CXX = ./src/cpp/extern_c.h ./src/cpp/Cli.h ./src/cpp/PolynomialConsoleGraph/src/Compute.h ./src/cpp/PolynomialConsoleGraph/src/Draw.h ./src/cpp/PolynomialConsoleGraph/src/Settings.h
OBJ = $(C_SRC:.c=.o) $(CXX_SRC:.cpp=.o)
EXEC = ./bin/nbs_dsp

# All
all: $(EXEC)

# Executable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Object Files
%.o: %.c $(HEADER_C)
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp $(HEADER_CXX)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
.PHONY: clean
clean:
	rm -f $(OBJ) $(EXEC)
