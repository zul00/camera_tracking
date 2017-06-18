# Simple makefile
# @auth: Zulkarnaen

# Binary name
PROJECT = camera_tracking
BIN = $(PROJECT).bin

# Specify source directory and object directory
OBJDIR = obj

# Specify libraries and flags
LIBS   = -lm
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
	gcc -o $(BIN) $(OBJ) $(CFLAGS) $(LIBS)
	ctags -R

# Rule for OBJECT
$(OBJDIR)/%.o: %.c $(DEPS) | $(OBJDIR)
	gcc -c -g $< -o $@

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
