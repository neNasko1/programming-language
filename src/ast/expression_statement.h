#pragma once

#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"

namespace grammar {

struct expression_statement : public statement {
    const std::unique_ptr<expression> expr;

    expression_statement(std::unique_ptr<expression> expr);
    ~expression_statement() = default;

    void print(std::ostream &out, const size_t identation) const;
	void compile(std::ostream &out, parsing::context &ctx);
};

};