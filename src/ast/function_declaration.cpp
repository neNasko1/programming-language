#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "../typing.h"
#include "function_declaration.h"

namespace grammar {

function_declaration::function_declaration(const std::string_view name, const std::string_view type_hint, std::unique_ptr<statement> body, std::vector<std::unique_ptr<function_declaration_parameter> > &params)
    : name(name != "main" ? name : "_start"), type_hint(type_hint), body(std::move(body)), type(typing::NOT_INFERED_ID), params(std::move(params)) {
}

void function_declaration::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "function " << this->name << " : " << this->type_hint << std::endl;
    for(const auto &param : this->params) {
        param->print(out, ident + 1);
    }
    this->body->print(out, ident);
}

void function_declaration::try_infering_type(parsing::context &ctx) {
    if(this->type != typing::NOT_INFERED_ID) { return; }

    const auto res = ctx.type_system.find_type(typing::string_comparator(this->type_hint));
    assert(res);

    this->type = res.value();
}

void function_declaration::emit_code(std::ostream &out, parsing::context &ctx) {
	const size_t ADDRESS_SIZE = 8;
	assert(this->name != "_start" || this->params.size() == 0);

	out << "\t ; function declaration " << this->name << std::endl;

	out << "\t global " << this->name << "\n";
	out << this->name << ":\n";

    this->args_size = 0;
    for(const auto &param : this->params) {
        param->emit_code(ctx);
    }
	ctx.func_stack_ptr = this->args_size + ADDRESS_SIZE; // The additional 8 comes from the call instruction using the stack

    out << "\t ; end of setup " << this->name << "\n" << std::endl;

	this->body->emit_code(out, ctx);

    if(this->name == "_start") {
		out << "_cleanup_" << this->name << ":\n";
        out << "\t add rsp, " << ctx.func_stack_ptr - ADDRESS_SIZE << "\n";

		out << "\t mov rax, 60\n";
		out << "\t syscall\n";
    } else {
        out << "_cleanup_" << this->name << ":\n";
        out << "\t add rsp, " << ctx.func_stack_ptr - this->args_size - ADDRESS_SIZE << "\n";

        out << "\t ret\n";
    }

    out << std::endl;
}

};
