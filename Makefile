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

clear:
	@echo "Removing all compiled files"
	@rm -r obj asm || :
	@rm $(EXECUTABLE) || :

run_compiler:
	@make
	@./compiler code.txt asm/test.asm && cat asm/test.asm
	@nasm -f elf64 -g -F dwarf -o asm/lib.o src/lib.asm
	@nasm -f elf64 -g -F dwarf -o asm/test.o asm/test.asm
	@gcc -no-pie -o asm/test asm/test.o asm/lib.o && ./asm/test && echo "Program exited with code" 0

recompile:
	@make rm
	@make test

