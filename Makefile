# Compiler to use
CXX = clang++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra -g -fsanitize=address

# Source files
SRCS = main.cpp tokenizer.cpp CompilationEngine.cpp utils.cpp

# Object files, automatically generated from SRCS
OBJS = $(SRCS:.cpp=.o)

# Name of the executable
TARGET = main

# Default target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Generic rule for compiling .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

# Clean up rule
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
