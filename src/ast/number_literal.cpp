#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "number_literal.h"

namespace grammar {

number_literal::number_literal(const std::string_view value) : value(value) {}

void number_literal::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "number(" << this->value << ")" << std::endl;
}

void number_literal::try_infering_type(parsing::context &context) {
    if(this->memory->type != typing::NOT_INFERED_ID) { return; }

    this->memory->type = typing::I64_ID;
}

void number_literal::emit_code(std::ostream &out, parsing::context &ctx) {
	this->try_infering_type(ctx);
	assert(this->memory->type != typing::NOT_INFERED_ID);

    const size_t INT64_SIZE = ctx.type_system.all_types[this->memory->type]->size;
	ctx.func_stack_ptr += INT64_SIZE;
	this->memory->stack_ptr = ctx.func_stack_ptr;

    out << "\t ; number_literal " << this->value << std::endl;

    out << "\t mov rax, " << this->value << "\n";
    out << "\t sub rsp, " << INT64_SIZE << "\n";
    out << "\t mov [rsp], rax\n"; // Push the value to the stack

    out << std::endl;
}

};