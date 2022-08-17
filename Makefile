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
	@rm -r obj || :
	@rm $(EXECUTABLE) || :

run_compiler:
	@make
	@mkdir -p asm
	@./compiler code.txt asm/test.asm && cat asm/test.asm
	@make compile_asm
	@./asm/test && echo "Program exited with code" 0

compile_asm:
	@nasm -f elf64 -g -o asm/lib.o src/lib.asm
	@nasm -f elf64 -g -o asm/test.o asm/test.asm
	@gcc -g -no-pie -o asm/test asm/test.o asm/lib.o

test_compiler:
	@./test.sh

recompile:
	@make clear
	@g++ $(SRC_FILES) $(CPPFLAGS) $(CXXFLAGS) -o $(EXECUTABLE)
	@make test_compiler

