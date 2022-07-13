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
    assert(this->init_value->type == typing::I64_ID);

    this->type = this->init_value->type;
    // TODO: Match type with type_hint

    assert(ctx.variables.find(this->name) == ctx.variables.end());
    ctx.create_variable(this->name, this);

    out << "mov rax, " << "[rsp+" << ctx.func_stack_ptr - this->init_value->stack_ptr << "]\n";

    const size_t VARIABLE_SIZE = ctx.type_system.all_types[this->init_value->type]->size;
	ctx.func_stack_ptr += VARIABLE_SIZE;
	this->stack_ptr = ctx.func_stack_ptr;
    out << "sub rsp, " << VARIABLE_SIZE << "\n";
    out << "mov [rsp], rax\n"; // Push the value to the stack

    out << std::endl;
}

};