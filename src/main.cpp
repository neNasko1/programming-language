#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "lexing.h"
#include "parsing.h"

int main(int argc, char *argv[]) {
    if(argc == 1) {
        std::cerr << "There is no file to compile" << std::endl;
        return -1;
    }

    // This is slow, as it probably does some copying -> fix or cope
    std::ifstream inputCode(argv[1]);
    std::stringstream buffer;
    buffer << inputCode.rdbuf();

    std::string code = buffer.str();
    lexing::lexer lexer(code);
    lexer.lex();

    parsing::parser parser(lexer);
    const auto program = std::move(parser.parse_program());

    program->print(std::cout, 0);

	return 0;
}
