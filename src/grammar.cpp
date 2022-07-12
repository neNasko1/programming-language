#include <iostream>
#include <string_view>
#include <assert.h>

#include "grammar.h"
#include "typing.h"
#include "parsing.h"

namespace grammar {

expression::expression() : type(typing::NOT_INFERED_ID) {}

number_literal::number_literal(const std::string_view value) : value(value) {}

void number_literal::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "number(" << this->value << ")" << std::endl;
}

void number_literal::try_infering_type(parsing::context &context) {
    if(this->type != typing::NOT_INFERED_ID) { return; }

    this->type = typing::I64_ID;
}

void number_literal::emit_code(std::ostream &out, parsing::context &ctx) {
	this->try_infering_type(ctx);
	assert(this->type != typing::NOT_INFERED_ID);

    const size_t INT64_SIZE = ctx.type_system.all_types[this->type]->size;
	ctx.func_stack_ptr += INT64_SIZE;
	this->stack_ptr = ctx.func_stack_ptr;

    out << "mov rax, " << this->value << "\n";
    out << "sub rsp, " << INT64_SIZE << "\n";
    out << "mov [rsp], rax\n"; // Push the value to the stack
}

identifier_literal::identifier_literal(const std::string_view value) : value(value) {}

void identifier_literal::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "identifier(" << this->value << ")" << std::endl;
}

void identifier_literal::try_infering_type(parsing::context &context) {
    // assert(false);
    if(this->type != typing::NOT_INFERED_ID) { return; }

    // TODO: Remove
    const auto res = context.get_variable_definition(typing::string_comparator(this->value));
    assert(res);

    const auto res_type = context.type_system.find_type(typing::string_comparator(std::string(res.value()->type)));
    assert(res_type.first);
	this->type = res_type.second;
}

void identifier_literal::emit_code(std::ostream &out, parsing::context &ctx) {
	assert(false);
}

binary_expression::binary_expression(std::unique_ptr<expression> lft, const lexing::token &op, std::unique_ptr<expression> rght)
    : lft(std::move(lft)), op(op), rght(std::move(rght)) {}

void binary_expression::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "binary_expression(" << std::endl;
    this->lft->print(out, ident + 1);
    out << tabulation << "\t" << this->op.value << std::endl;
    this->rght->print(out, ident + 1);
    out << tabulation << ")" << std::endl;
}

void binary_expression::try_infering_type(parsing::context &context) {
	this->lft->try_infering_type(context);
	this->rght->try_infering_type(context);

	assert(this->lft->type == this->rght->type);
	this->type = this->lft->type;
}

void binary_expression::emit_code(std::ostream &out, parsing::context &ctx) {
	this->try_infering_type(ctx);
	assert(this->type != typing::NOT_INFERED_ID);

	assert(this->type == typing::I64_ID); // TODO: Handle different types of expressions

	this->lft->emit_code(out, ctx);
	this->rght->emit_code(out, ctx);

	switch(this->op.type) {
		case lexing::token_type::PLUS: {
			out << "mov " << " rax, " << "[rsp+" << ctx.func_stack_ptr - this->lft->stack_ptr << "]\n";
			out << "add " << " rax, " << "[rsp+" << ctx.func_stack_ptr - this->rght->stack_ptr << "]\n";

            const size_t TYPE_SIZE = ctx.type_system.all_types[this->type]->size;
			ctx.func_stack_ptr += TYPE_SIZE;
			this->stack_ptr = ctx.func_stack_ptr;

            out << "sub rsp," << TYPE_SIZE << "\n";
            out << "mov [rsp], rax\n"; // Push the value to the stack

			break;
		}
	}
}

expression_statement::expression_statement(std::unique_ptr<expression> expr) : expr(std::move(expr)) {}

void expression_statement::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "expression_statement(" << std::endl;
    this->expr->print(out, ident + 1);
    out << tabulation << ")" << std::endl;
}

void expression_statement::emit_code(std::ostream &out, parsing::context &ctx) {
	this->expr->emit_code(out, ctx);
}

list_statement::list_statement(std::vector<std::unique_ptr<statement> > &body) : body(std::move(body)) {}

void list_statement::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "{" << std::endl;
    for(const auto &it : this->body) {
        it->print(out, ident + 1);
    }
    out << tabulation << "}" << std::endl;
}

void list_statement::emit_code(std::ostream &out, parsing::context &ctx) {
	for(const auto &stat : this->body) {
		stat->emit_code(out, ctx);
	}
}

let_statement::let_statement(const std::string_view name, std::unique_ptr<expression> init_value) : name(name), type(""), init_value(std::move(init_value)) {}

let_statement::let_statement(const std::string_view name, const std::string_view type, std::unique_ptr<expression> init_value) : name(name), type(type), init_value(std::move(init_value)) {}

void let_statement::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    if(this->type.size() == 0) {
        out << tabulation << "let " << this->name << ": " << "auto" << std::endl;
    } else {
        out << tabulation << "let " << this->name << ": " << this->type << std::endl;
    }
    this->init_value->print(out, ident + 1);
}

void let_statement::emit_code(std::ostream &out, parsing::context &ctx) {
	assert(false);
}

return_statement::return_statement(std::unique_ptr<expression> value) : value(std::move(value)) {}

void return_statement::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "return " << std::endl;
    this->value->print(out, ident + 1);
}

void return_statement::emit_code(std::ostream &out, parsing::context &ctx) {
	this->value->emit_code(out, ctx);

	if(ctx.current_declaration->name == "_start") {
		out << "mov rdi, " << "[rsp+" << ctx.func_stack_ptr - this->value->stack_ptr << "]\n";
		out << "mov rax, 60\n";
		out << "syscall\n";
	} else {
        assert(false);
    }
}

function_declaration::function_declaration(const std::string_view name, const std::string_view type, std::unique_ptr<statement> body)
    : name(name != "main" ? name : "_start"), type(type), body(std::move(body)) {
}

void function_declaration::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "function " << this->name << " : " << this->type << std::endl;
    this->body->print(out, ident);
}

void function_declaration::emit_code(std::ostream &out, parsing::context &ctx) {
	out << "global " << this->name << "\n";
	out << this->name << ":\n";

	this->body->emit_code(out, ctx);
}


program::program(std::vector<std::unique_ptr<function_declaration> > &function_declarations) : function_declarations(std::move(function_declarations)) {}

void program::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "program(" << std::endl;
    for(const auto &it : this->function_declarations) {
        it->print(out, ident + 1);
    }
    out << tabulation << ")" << std::endl;
}

void program::emit_code(std::ostream &out, parsing::context &ctx) {
	out << "section .text\n";
	for(const auto &decl : this->function_declarations) {
		ctx.current_declaration = decl.get();
		decl->emit_code(out, ctx);
		// TODO: Stack pointer should be cleared up
	}
}

};
