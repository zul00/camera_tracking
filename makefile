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
CFLAGS = -Wall -Wextra -g

# List C files
SRC = $(wildcard *.cpp)

# List dependencies
DEP = $(wildcard *.h)

# Target O files, at OBJDIR
_OBJ = $(patsubst %.cpp, %.o, $(SRC))
OBJ = $(patsubst %, $(OBJDIR)/%, $(_OBJ))

# Main Rule
all: $(BIN)

# Rule for BINARY
$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(CFLAGS) 	
	#ctags -R

# Rule for OBJECT
$(OBJDIR)/%.o: %.cpp $(DEPS) | $(OBJDIR)
	$(CC) -c -g $< -o $@ $(LDLIBS)

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
