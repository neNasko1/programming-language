#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "../typing.h"
#include "let_statement.h"

namespace grammar {

let_statement::let_statement(const std::string_view name, std::unique_ptr<expression> init_value) : name(name), type_hint(""), init_value(std::move(init_value)) {}

let_statement::let_statement(const std::string_view name, const std::string_view type_hint, std::unique_ptr<expression> init_value) : name(name), type_hint(type_hint), init_value(std::move(init_value)) {}

void let_statement::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    if(this->type_hint.size() == 0) {
        out << tabulation << "let " << this->name << ": " << "auto" << std::endl;
    } else {
        out << tabulation << "let " << this->name << ": " << this->type_hint << std::endl;
    }
    this->init_value->print(out, ident + 1);
}

void let_statement::emit_code(std::ostream &out, parsing::context &ctx) {
    this->init_value->emit_code(out, ctx);
	assert(this->init_value->memory->type == typing::I64_ID);

    assert(this->init_value->memory->type == ctx.type_system.find_type(typing::string_comparator(this->type_hint)));

    // this->type = this->init_value->type;
    // TODO: Match type with type_hint

    assert(ctx.variables.find(this->name) == ctx.variables.end()); // Assert this is the first declaration of the variable

    out << "\t push qword " << "[rsp+" << ctx.func_stack_ptr - this->init_value->memory->stack_ptr << "]\n";

    const size_t VARIABLE_SIZE = ctx.type_system.all_types[this->init_value->memory->type]->size;
	ctx.func_stack_ptr += VARIABLE_SIZE;

    this->memory = std::make_unique<memory_cell>(ctx.func_stack_ptr, this->init_value->memory->type);
    ctx.declare_variable(this->name, this->memory.get());

    out << std::endl;
}

};
