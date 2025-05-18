# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

# Target executable
TARGET = sim

# Source files
SRCS = main.cpp process.cpp burst.cpp event.cpp simulator.cpp statistics.cpp generator.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Header files
HDRS = types.h process.h burst.h event.h simulator.h statistics.h generator.h

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJS) $(TARGET)

# Run target
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run