# programming-language
Simple programing language written in C++ with speed in mind

# Features
Currently the compiler is in its toy stage - some essential features are missing, such as assignment and boolean operations. This is done, because at this stage they are essentially trivial to implement. 
Some of the supported features include type-checking(integers, booleans and void), function-calling, recursion and external(wrapped in lib.asm and linked with libc) functions.

# Unit-tests and examples
It is trivial to add new unit-tests, which serve a second purpose as examples:
## Add a new source file in unit_tests/input/
```
cp to_test.lf unit_tests/input/to_test.lf
```
## Run test.sh
```
./test.sh
```

# Future plans(In this order)
1. Add reference types
2. Rawptr
3. Arrays as malloc/free calls
4. Templated functions
5. Structured types
6. Templated structures
