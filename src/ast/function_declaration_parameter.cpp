#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "../typing.h"
#include "function_declaration_parameter.h"

namespace grammar {

function_declaration_parameter::function_declaration_parameter(const std::string &name, std::unique_ptr<type_call> &type)
    : name(name), type(std::move(type)) {
}

void function_declaration_parameter::print(std::ostream &out, const size_t ident) const {
    const std::string tabulation = std::string(ident, '\t');

    out << tabulation << this->name << " : " << this->type->internal_name << std::endl;
}

void function_declaration_parameter::compile(std::ostream &out, parsing::context &ctx) {
	this->type->compile(out, ctx);

	const auto type_ind = this->type->type_ind;

	ctx.current_declaration->args_size += ctx.type_system.all_types[type_ind]->size;
    this->memory = std::make_unique<memory_cell>(ctx.current_declaration->return_size + ctx.current_declaration->args_size, type_ind);
    ctx.declare_variable(this->name, this->memory.get());
}

};
