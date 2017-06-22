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
CC = gcc

# Specify library
CFLAGS = -Wall -Wextra -g

# List C files
SRC = $(wildcard *.c)

# List dependencies
DEP = $(wildcard *.h)

# Target O files, at OBJDIR
_OBJ = $(patsubst %.c, %.o, $(SRC))
OBJ = $(patsubst %, $(OBJDIR)/%, $(_OBJ))

# Main Rule
all: $(BIN)

# Rule for BINARY
$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(CFLAGS) 	
	#ctags -R

# Rule for OBJECT
$(OBJDIR)/%.o: %.c $(DEPS) | $(OBJDIR)
	$(CC) -c -g $< -o $@ $(LDLIBS)

# Create OBJDIR if not exist
$(OBJDIR):
	mkdir $(OBJDIR)

.PHONY: clean
clean:
	rm $(OBJ) $(BIN)

.PHONY: print
print:
	@echo src: $(SRC)
	@echo obj: $(OBJ)
	@echo dep: $(DEP)
	@echo bin: $(BIN)
