#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "identifier_literal.h"

namespace grammar {

identifier_literal::identifier_literal(const std::string_view name) : name(name) {}

void identifier_literal::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "identifier(" << this->name << ")" << std::endl;
}

void identifier_literal::try_infering_type(parsing::context &context) {
    if(this->type != typing::NOT_INFERED_ID) { return; }

    const auto res = context.get_variable_definition(typing::string_comparator(this->name));
    assert(res);
    this->definition = res.value();

	this->type = this->definition->type;
}

void identifier_literal::emit_code(std::ostream &out, parsing::context &ctx) {
    this->try_infering_type(ctx);

    assert(this->type == typing::I64_ID);

    out << "\t ; identifier_literal " << this->name << std::endl;

    out << "\t mov rax, " << "[rsp+" << ctx.func_stack_ptr - this->definition->stack_ptr << "]\n";

    const size_t VARIABLE_SIZE = ctx.type_system.all_types[this->type]->size;
	ctx.func_stack_ptr += VARIABLE_SIZE;
	this->stack_ptr = ctx.func_stack_ptr;
    out << "\t sub rsp, " << VARIABLE_SIZE << "\n";
    out << "\t mov [rsp], rax\n"; // Push the value to the stack    

    out << std::endl;
}

};