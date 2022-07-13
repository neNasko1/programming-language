#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "lexing.h"
#include "parsing.h"

int main(int argc, char **argv) {
    if(argc != 3) {
        std::cerr << "There are no files for input and output" << std::endl;
        return -1;
    }

    // This is slow, as it probably does some copying -> fix or cope
    std::ifstream input_code(argv[1]);
    std::stringstream buffer;
    buffer << input_code.rdbuf();

    std::string code = buffer.str();
    lexing::lexer lexer(code);
    lexer.lex();

    parsing::parser parser(lexer);
    const auto program = std::move(parser.parse_program());

    program->print(std::cout, 0);

	std::ofstream output_code(argv[2]);
	parsing::context ctx;
	program->emit_code(output_code, ctx);
	output_code.close();

	return 0;
}
