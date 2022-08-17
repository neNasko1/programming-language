#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "identifier_literal.h"

namespace grammar {

identifier_literal::identifier_literal(const std::string &name) : name(name) {}

void identifier_literal::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "identifier(" << this->name << ")" << std::endl;
}

void identifier_literal::try_infering_type(parsing::context &context) {
    if(this->memory->type != typing::NOT_INFERED_ID) { return; }

    const auto res = context.get_variable_definition(std::string(this->name));
    assert(res);

    this->memory->stack_ptr = res.value()->stack_ptr;
    this->memory->type = res.value()->type;
}

void identifier_literal::emit_code(std::ostream &out, parsing::context &ctx) {
    this->try_infering_type(ctx);
}

};
