#pragma once

#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"

namespace grammar {

struct function_call : public expression {
    const std::string name;
    const std::vector<std::unique_ptr<expression> > args;

    const function_declaration* definition;

    function_call(const std::string &name, std::vector<std::unique_ptr<expression> > args);

    void print(std::ostream &out, const size_t ident) const;
    void try_infering_type(parsing::context &ctx);
	void compile(std::ostream &out, parsing::context &ctx);
};

};