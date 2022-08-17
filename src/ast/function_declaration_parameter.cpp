#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "../typing.h"
#include "function_declaration_parameter.h"

namespace grammar {

function_declaration_parameter::function_declaration_parameter(const std::string &name, const std::string &type_hint)
    : name(name), type_hint(type_hint), type(typing::NOT_INFERED_ID) {
}

void function_declaration_parameter::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << this->name << " : " << this->type_hint << std::endl;
}

void function_declaration_parameter::emit_code(parsing::context &ctx) {
    if(this->type != typing::NOT_INFERED_ID) { return; }

    const auto res = ctx.type_system.find_type(this->type_hint);
    assert(res);

    this->type = res.value();

	ctx.current_declaration->args_size += ctx.type_system.all_types[this->type]->size;
    this->memory = std::make_unique<memory_cell>(ctx.current_declaration->return_size + ctx.current_declaration->args_size, this->type);
    ctx.declare_variable(this->name, this->memory.get());
}

};
