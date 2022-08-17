#pragma once

#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"

namespace grammar {

struct return_statement : public statement {
    const std::unique_ptr<expression> value;

    return_statement(std::unique_ptr<expression> value);
    ~return_statement() = default;

    void print(std::ostream &out, const size_t identation) const;
	void compile(std::ostream &out, parsing::context &ctx);
};

};