#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "../typing.h"
#include "../asm_helper.h"
#include "let_statement.h"

namespace grammar {

let_statement::let_statement(const std::string &name, std::unique_ptr<type_call> &type, std::unique_ptr<expression> init_value) : name(name), type(std::move(type)), init_value(std::move(init_value)) {}

void let_statement::print(std::ostream &out, const size_t ident) const {
    const std::string tabulation = std::string(ident, '\t');

    out << tabulation << "let " << this->name << " : " << this->type->internal_name << std::endl;
    this->init_value->print(out, ident + 1);
}

void let_statement::compile(std::ostream &out, parsing::context &ctx) {
    this->init_value->compile(out, ctx);

    // this->type = this->init_value->type;
    // TODO: Match type with type_hint

    assert(ctx.variables.find(this->name) == ctx.variables.end()); // Assert this is the first declaration of the variable

    const size_t VARIABLE_SIZE = ctx.type_system.all_types[this->init_value->memory->type]->size;
	asm_helper::push_to_stack(out, ctx.func_stack_ptr - this->init_value->memory->stack_ptr, VARIABLE_SIZE);
	ctx.func_stack_ptr += VARIABLE_SIZE;

    this->memory = std::make_unique<memory_cell>(ctx.func_stack_ptr, this->init_value->memory->type);
    ctx.declare_variable(this->name, this->memory.get());

    out << std::endl;
}

};
