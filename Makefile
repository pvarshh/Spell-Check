# Makefile for Spell Checker

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O3 -DNDEBUG
DEBUG_FLAGS = -std=c++17 -Wall -Wextra -Wpedantic -g -O0 -DDEBUG
INCLUDES = -Iinclude

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TARGET = $(BUILD_DIR)/spell_checker

# Default target
all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Create object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Build target
$(TARGET): $(BUILD_DIR) $(OBJ_DIR) $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Debug build
debug: CXXFLAGS = $(DEBUG_FLAGS)
debug: $(TARGET)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Install target
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/spell_checker
	mkdir -p /usr/local/share/spell_checker/dictionaries
	cp dictionaries/en_US.dict /usr/local/share/spell_checker/dictionaries/

# Uninstall target
uninstall:
	rm -f /usr/local/bin/spell_checker
	rm -rf /usr/local/share/spell_checker

# Run tests (if you want to add a simple test)
test: $(TARGET)
	@echo "Running basic tests..."
	@echo "Testing word check:"
	@echo "hello" | $(TARGET) -w "hello" && echo "✓ Correct word test passed" || echo "✗ Correct word test failed"
	@echo "Testing misspelled word:"
	@$(TARGET) -w "helllo" | grep -q "Suggestions" && echo "✓ Misspelled word test passed" || echo "✗ Misspelled word test failed"

# Help target
help:
	@echo "Available targets:"
	@echo "  all      - Build the spell checker (default)"
	@echo "  debug    - Build with debug information"
	@echo "  clean    - Remove build files"
	@echo "  install  - Install to /usr/local/bin"
	@echo "  uninstall- Remove from /usr/local/bin"
	@echo "  test     - Run basic tests"
	@echo "  help     - Show this help message"

# Phony targets
.PHONY: all debug clean install uninstall test help

# Dependencies (simplified - in a real project you'd generate these)
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp $(INCLUDE_DIR)/spell_checker.h
$(OBJ_DIR)/spell_checker.o: $(SRC_DIR)/spell_checker.cpp $(INCLUDE_DIR)/spell_checker.h $(INCLUDE_DIR)/dictionary.h $(INCLUDE_DIR)/suggestion_engine.h $(INCLUDE_DIR)/text_processor.h
$(OBJ_DIR)/dictionary.o: $(SRC_DIR)/dictionary.cpp $(INCLUDE_DIR)/dictionary.h
$(OBJ_DIR)/suggestion_engine.o: $(SRC_DIR)/suggestion_engine.cpp $(INCLUDE_DIR)/suggestion_engine.h $(INCLUDE_DIR)/dictionary.h
$(OBJ_DIR)/text_processor.o: $(SRC_DIR)/text_processor.cpp $(INCLUDE_DIR)/text_processor.h
