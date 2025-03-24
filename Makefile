CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wno-write-strings -Wno-address
LDFLAGS = -pthread

SRCDIR = src
INCDIR = include
OBJDIR = obj

# Create object directory if it doesn't exist
$(shell mkdir -p $(OBJDIR))

# Core source files (excluding main programs)
CORE_SOURCES = $(filter-out $(SRCDIR)/main.cpp $(SRCDIR)/main_web.cpp, $(wildcard $(SRCDIR)/*.cpp))
CORE_OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CORE_SOURCES))

# CLI application
CLI_EXECUTABLE = file_system
CLI_OBJECTS = $(CORE_OBJECTS) $(OBJDIR)/main.o

# Web application
WEB_EXECUTABLE = file_system_web
WEB_OBJECTS = $(CORE_OBJECTS) $(OBJDIR)/main_web.o

# Default build both applications
all: $(CLI_EXECUTABLE) $(WEB_EXECUTABLE)

# Build CLI executable
$(CLI_EXECUTABLE): $(CLI_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CLI_OBJECTS) -o $@ $(LDFLAGS)

# Build Web executable
$(WEB_EXECUTABLE): $(WEB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(WEB_OBJECTS) -o $@ $(LDFLAGS) -pthread

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

# Clean up
clean:
	rm -rf $(OBJDIR) $(CLI_EXECUTABLE) $(WEB_EXECUTABLE)

# Run the CLI application
run: $(CLI_EXECUTABLE)
	./$(CLI_EXECUTABLE)

# Run the Web application
run-web: $(WEB_EXECUTABLE)
	./$(WEB_EXECUTABLE)

# Just build CLI application
cli: $(CLI_EXECUTABLE)

# Just build Web application
web: $(WEB_EXECUTABLE)

.PHONY: all clean run run-web cli web
