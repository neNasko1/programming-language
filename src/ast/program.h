#pragma once

#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"

namespace grammar {

struct program : public ast_node {
    const std::vector<std::unique_ptr<function_declaration> > function_declarations;

    program(std::vector<std::unique_ptr<function_declaration> > &function_declarations);
    ~program() = default;

	void print(std::ostream &out, const size_t identation) const;
	void compile(std::ostream &out, parsing::context &ctx);
};

};