# Simple makefile
# @auth: Zulkarnaen

# Binary name
PROJECT = camera_tracking
BIN = $(PROJECT).bin

# Specify source directory and object directory
OBJDIR = obj

# Compiler
#CROSS_COMPILER = arm-linux-gnueabi-gcc
#CC = $(CROSS_COMPILER)gcc
CC = g++

# Specify library
CFLAGS += -c -Wall -Wextra $(shell pkg-config --cflags opencv)
LDFLAGS += $(shell pkg-config --libs --static opencv)

# List C files
SRC = $(wildcard *.cpp)

# List dependencies
DEP = $(wildcard *.hpp)

# Target O files, at OBJDIR
_OBJ = $(patsubst %.cpp, %.o, $(SRC))
OBJ = $(patsubst %, $(OBJDIR)/%, $(_OBJ))

# Main Rule
all: $(BIN)

# Rule for BINARY
$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDFLAGS)
	ctags -R

# Rule for OBJECT
$(OBJDIR)/%.o: %.cpp $(DEPS) | $(OBJDIR)
	$(CC) -c -g $< -o $@ $(CFLAGS)

# Create OBJDIR if not exist
$(OBJDIR):
	mkdir $(OBJDIR)

.PHONY: run
run: $(BIN)
	./$(BIN)

.PHONY: clean
clean:
	rm $(OBJ) $(BIN)

.PHONY: print
print:
	@echo src: $(SRC)
	@echo obj: $(OBJ)
	@echo dep: $(DEP)
	@echo bin: $(BIN)
