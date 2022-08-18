#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "binary_expression.h"

namespace grammar {

binary_expression::binary_expression(std::unique_ptr<expression> lft, const lexing::token &op, std::unique_ptr<expression> rght)
    : lft(std::move(lft)), op(op), rght(std::move(rght)) {}

void binary_expression::print(std::ostream &out, const size_t ident) const {
    const std::string tabulation = std::string(ident, '\t');

    out << tabulation << "binary_expression(" << std::endl;
    this->lft->print(out, ident + 1);
    out << tabulation << "\t" << this->op.value << std::endl;
    this->rght->print(out, ident + 1);
    out << tabulation << ")" << std::endl;
}

void binary_expression::try_infering_type(parsing::context &ctx) {
	this->lft->try_infering_type(ctx);
	this->rght->try_infering_type(ctx);

	// Check if this is i64 or &<i64>
	const auto lft_tid = this->lft->memory->type_ind;
	const auto lft_uref = ctx.type_system.unwrap_ref(lft_tid);
	assert(lft_uref.first == typing::I64_ID);

	// Check if this is i64 or &<i64>
	const auto rght_tid = this->rght->memory->type_ind;
	const auto rght_uref = ctx.type_system.unwrap_ref(rght_tid);
	assert(lft_uref.first == typing::I64_ID);

	this->memory->type_ind = typing::I64_ID;
}

void binary_expression::compile(std::ostream &out, parsing::context &ctx) {
	this->try_infering_type(ctx);
	assert(this->memory->type_ind == typing::I64_ID); // TODO: Handle different types of expressions

	this->lft->compile(out, ctx);
	this->rght->compile(out, ctx);

	const auto lft_tid = this->lft->memory->type_ind;
	const auto lft_uref = ctx.type_system.unwrap_ref(lft_tid);

	const auto rght_tid = this->rght->memory->type_ind;
	const auto rght_uref = ctx.type_system.unwrap_ref(rght_tid);

	out << "\t ; binary_expression " << lexing::reverse_token_type_names[this->op.type] << std::endl;

	if(lft_uref.second) {
		out << "\t lea rcx, [rsp+" << ctx.func_stack_ptr - this->lft->memory->stack_ptr << "]\n";
		for(size_t i = 0; i < lft_uref.second; i ++) {
			out << "\t mov rcx, [rcx]\n";
		}
		out << "\t mov rax, [rcx]\n";
	} else {
		out << "\t mov rax, [rsp+" << ctx.func_stack_ptr - this->lft->memory->stack_ptr << "]\n";
	}

	if(rght_uref.second) {
		out << "\t lea rcx, [rsp+" << ctx.func_stack_ptr - this->rght->memory->stack_ptr << "]\n";
		for(size_t i = 0; i < rght_uref.second; i ++) {
			out << "\t mov rcx, [rcx]\n";
		}
		out << "\t mov rcx, [rcx]\n";
	} else {
		out << "\t mov rcx, [rsp+" << ctx.func_stack_ptr - this->rght->memory->stack_ptr << "]\n";
	}


	switch(this->op.type) {
		case lexing::token_type::PLUS: {
			out << "\t add rax, rcx\n";

            const size_t TYPE_SIZE = ctx.type_system.all_types[this->memory->type_ind]->size;
			ctx.func_stack_ptr += TYPE_SIZE;
			this->memory->stack_ptr = ctx.func_stack_ptr;
            out << "\t push rax" << "\n";

			break;
		}
		case lexing::token_type::MINUS: {
			out << "\t sub rax, rcx\n";

            const size_t TYPE_SIZE = ctx.type_system.all_types[this->memory->type_ind]->size;
			ctx.func_stack_ptr += TYPE_SIZE;
			this->memory->stack_ptr = ctx.func_stack_ptr;
            out << "\t push rax" << "\n";

			break;
		}
		case lexing::token_type::STAR: {
			out << "\t mul rcx\n";

            const size_t TYPE_SIZE = ctx.type_system.all_types[this->memory->type_ind]->size;
			ctx.func_stack_ptr += TYPE_SIZE;
			this->memory->stack_ptr = ctx.func_stack_ptr;
            out << "\t push rax" << "\n";

			break;
		}
		case lexing::token_type::SLASH: {
			out << "\t div rcx\n";

            const size_t TYPE_SIZE = ctx.type_system.all_types[this->memory->type_ind]->size;
			ctx.func_stack_ptr += TYPE_SIZE;
			this->memory->stack_ptr = ctx.func_stack_ptr;
            out << "\t push rax" << "\n";

			break;
		}
		case lexing::token_type::MODULO: {
			out << "\t div rcx\n";

            const size_t TYPE_SIZE = ctx.type_system.all_types[this->memory->type_ind]->size;
			ctx.func_stack_ptr += TYPE_SIZE;
			this->memory->stack_ptr = ctx.func_stack_ptr;
            out << "\t push rdx" << "\n";

			break;
		}
		default: {
			assert(false);
		}
	}

    out << std::endl;
}


};
