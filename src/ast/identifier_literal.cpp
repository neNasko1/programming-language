#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "identifier_literal.h"

namespace grammar {

identifier_literal::identifier_literal(const std::string_view value) : value(value) {}

void identifier_literal::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "identifier(" << this->value << ")" << std::endl;
}

void identifier_literal::try_infering_type(parsing::context &context) {
    // assert(false);
    if(this->type != typing::NOT_INFERED_ID) { return; }

    // TODO: Remove
    const auto res = context.get_variable_definition(typing::string_comparator(this->value));
    assert(res);

    const auto res_type = context.type_system.find_type(typing::string_comparator(std::string(res.value()->type)));
    assert(res_type.first);
	this->type = res_type.second;
}

void identifier_literal::emit_code(std::ostream &out, parsing::context &ctx) {
	assert(false);
}

};