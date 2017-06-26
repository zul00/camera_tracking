# Simple makefile
# @auth: Zulkarnaen

# Binary name
PROJECT = camera_tracking
BIN = $(PROJECT).bin

# Specify source directory and object directory
OBJDIR = obj

# Compiler
#CROSS_COMPILER = arm-linux-gnueabi-
#CC = $(CROSS_COMPILER)gcc
CC = gcc

# Specify library
KERNEL_MODULE_DIR=$(PWD)/../kernel_module
CFLAGS += -c -Wall -Wextra -lm $(shell pkg-config --cflags opencv) -I$(KERNEL_MODULE_DIR)/include
LDFLAGS += $(shell pkg-config --libs --static opencv) -lm


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
	$(CC) -o $(BIN) $(OBJ) $(LDFLAGS)
	ctags -R

# Rule for OBJECT
$(OBJDIR)/%.o: %.c $(DEP) | $(OBJDIR)
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
