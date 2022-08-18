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

    out << tabulation << "let " << this->name << " : " << (this->type ? this->type->internal_name : "auto") << std::endl;
    this->init_value->print(out, ident + 1);
}

void let_statement::compile(std::ostream &out, parsing::context &ctx) {
	if(this->type) {
		this->type->compile(out, ctx);
	}
	this->init_value->compile(out, ctx);

	if(this->type) {
		assert(this->type->type_ind == this->init_value->memory->type_ind);
	}
    // this->type = this->init_value->type;
    // TODO: Match type with type_hint

    assert(ctx.variables.find(this->name) == ctx.variables.end()); // Assert this is the first declaration of the variable

    this->memory = std::make_unique<memory_cell>(this->init_value->memory->stack_ptr, this->init_value->memory->type_ind);
    ctx.declare_variable(this->name, this->memory.get());

    out << std::endl;
}

};
