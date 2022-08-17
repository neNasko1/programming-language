#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "expression_statement.h"

namespace grammar {

expression_statement::expression_statement(std::unique_ptr<expression> expr) : expr(std::move(expr)) {}

void expression_statement::print(std::ostream &out, const size_t ident) const {
    const std::string tabulation = std::string(ident, '\t');

    out << tabulation << "expression_statement(" << std::endl;
    this->expr->print(out, ident + 1);
    out << tabulation << ")" << std::endl;
}

void expression_statement::compile(std::ostream &out, parsing::context &ctx) {
	this->expr->compile(out, ctx);
}

};