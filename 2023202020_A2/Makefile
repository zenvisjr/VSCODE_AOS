# Name of the target executable
TARGET = endgame

# Source files
SRCS = posix_final.cpp

# Object files generated from source files
OBJS = $(SRCS:.cpp=.o)

# Compiler to use
CXX = g++

# Flags for the compiler
CXXFLAGS = -Wall -std=c++17

# Libraries to link against
LIBS = -lreadline -lstdc++fs

# Rule for making the target
$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS)

# Rule for cleaning up generated files
clean:
	rm -f $(OBJS) $(TARGET)

