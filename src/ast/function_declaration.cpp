#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "../typing.h"
#include "function_declaration.h"

namespace grammar {

function_declaration::function_declaration(const std::string &name, const std::string &type_hint, std::unique_ptr<statement> body, std::vector<std::unique_ptr<function_declaration_parameter> > &params)
    : name(name), type_hint(type_hint), body(std::move(body)), type_ind(typing::NOT_INFERED_ID), params(std::move(params)) {
}

function_declaration::function_declaration(const std::string &name, const std::string &type_hint, std::vector<std::unique_ptr<function_declaration_parameter> > &params)
    : name(name), type_hint(type_hint), body(nullptr), type_ind(typing::NOT_INFERED_ID), params(std::move(params)) {
}

void function_declaration::print(std::ostream &out, const size_t ident) const {
    const std::string tabulation = std::string(ident, '\t');

    out << tabulation << "function " << this->name << " : " << this->type_hint << std::endl;
    for(const auto &param : this->params) {
        param->print(out, ident + 1);
    }

	if(this->body) {
    	this->body->print(out, ident);
	}
}

void function_declaration::try_infering_type(parsing::context &ctx) {
    if(this->type_ind != typing::NOT_INFERED_ID) { return; }

    const auto res = ctx.type_system.find_type(this->type_hint);
    assert(res);

    this->type_ind = res.value();
}

void function_declaration::compile(std::ostream &out, parsing::context &ctx) {
	this->try_infering_type(ctx);

	this->return_size = ctx.type_system.all_types[this->type_ind]->size;

    this->args_size = 0;
	for(int i = this->params.size() - 1; i >= 0; i --) {
        this->params[i]->compile(out, ctx);
    }

	if(!this->body) {
		// This is an extern function so skip compiling;
		out << "\t extern " << this->name << "\n";
		return;
	}

	assert(this->name != "main" || this->params.size() == 0);
	out << "\t ; function declaration " << this->name << std::endl;
	out << "\t global " << this->name << "\n";
	out << this->name << ":\n";

	const size_t ADDRESS_SIZE = 8;
	ctx.func_stack_ptr += this->return_size;
	ctx.func_stack_ptr += this->args_size;
	ctx.func_stack_ptr += ADDRESS_SIZE; // The additional 8 comes from the call instruction using the stack

	out << "\t push rbp\n";
	ctx.func_stack_ptr += 8;
	out << "\t mov rbp, rsp\n";

    out << "\t ; end of setup " << this->name << "\n" << std::endl;

	this->body->compile(out, ctx);

	out << "_cleanup_" << this->name << ":\n";
	out << "\t mov rsp, rbp\n";
	out << "\t pop rbp\n";
	ctx.func_stack_ptr -= 8;

    if(this->name == "main") {
		out << "\t mov rax, 60\n";
		out << "\t syscall\n";
    } else {
        out << "\t ret\n";
    }

    out << std::endl;
}

};
