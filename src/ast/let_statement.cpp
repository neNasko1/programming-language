#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "let_statement.h"

namespace grammar {

let_statement::let_statement(const std::string_view name, std::unique_ptr<expression> init_value) : name(name), type(""), init_value(std::move(init_value)) {}

let_statement::let_statement(const std::string_view name, const std::string_view type, std::unique_ptr<expression> init_value) : name(name), type(type), init_value(std::move(init_value)) {}

void let_statement::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    if(this->type.size() == 0) {
        out << tabulation << "let " << this->name << ": " << "auto" << std::endl;
    } else {
        out << tabulation << "let " << this->name << ": " << this->type << std::endl;
    }
    this->init_value->print(out, ident + 1);
}

void let_statement::emit_code(std::ostream &out, parsing::context &ctx) {
	assert(false);
}

};