SRC_DIR := src
OBJ_DIR := obj
SRC_FILES := $(shell find src/ -type f -name '*.cpp')
OBJ_FILES := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -fsanitize=address
CPPFLAGS := -std=c++2a -fsanitize=address
CXXFLAGS := -fsanitize=address
EXECUTABLE := compiler

$(EXECUTABLE): $(OBJ_FILES)
	g++ $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p "$$(dirname $@)"
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

rm:
	@echo "Removing all compiled files"
	@rm -r obj || :
	@rm $(EXECUTABLE) || :

test:
	@make
	@./test.sh

debug:
	@./compiler code.txt compiled.asm
	@nasm -f elf64 -g -F dwarf -o compiled.o compiled.asm && ld -o compiled compiled.o && gdb compiled -tui

recompile:
	@make rm
	@make test

