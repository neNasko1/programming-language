#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "binary_expression.h"

namespace grammar {

binary_expression::binary_expression(std::unique_ptr<expression> &lft, const lexing::token &op, std::unique_ptr<expression> &rght)
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

	// TODO: Skip this function
	this->memory->type_ind = typing::I64_ID;
}

void binary_expression::compile(std::ostream &out, parsing::context &ctx) {
	this->try_infering_type(ctx);

	this->lft->compile(out, ctx);
	this->rght->compile(out, ctx);

	out << "\t ; binary_expression " << lexing::reverse_token_type_names[this->op.type] << std::endl;

	const auto load_rax_rcx_arithmetic = [&]() {
		const auto lft_tid = this->lft->memory->type_ind;
		const auto lft_uref = ctx.type_system.unwrap_ref(lft_tid);

		assert(lft_uref.first == typing::I64_ID);

		if(lft_uref.second) {
			out << "\t lea rcx, [rsp+" << ctx.func_stack_ptr - this->lft->memory->stack_ptr << "]\n";
			for(size_t i = 0; i < lft_uref.second; i ++) {
				out << "\t mov rcx, [rcx]\n";
			}
			out << "\t mov rax, [rcx]\n";
		} else {
			out << "\t mov rax, [rsp+" << ctx.func_stack_ptr - this->lft->memory->stack_ptr << "]\n";
		}

		const auto rght_tid = this->rght->memory->type_ind;
		const auto rght_uref = ctx.type_system.unwrap_ref(rght_tid);

		assert(rght_uref.first == typing::I64_ID);

		if(rght_uref.second) {
			out << "\t lea rcx, [rsp+" << ctx.func_stack_ptr - this->rght->memory->stack_ptr << "]\n";
			for(size_t i = 0; i < rght_uref.second; i ++) {
				out << "\t mov rcx, [rcx]\n";
			}
			out << "\t mov rcx, [rcx]\n";
		} else {
			out << "\t mov rcx, [rsp+" << ctx.func_stack_ptr - this->rght->memory->stack_ptr << "]\n";
		}

		this->memory->type_ind = typing::I64_ID;
	};

	switch(this->op.type) {
		case lexing::token_type::PLUS: {
			load_rax_rcx_arithmetic();
			out << "\t add rax, rcx\n";

            const size_t TYPE_SIZE = ctx.type_system.all_types[this->memory->type_ind]->size;
			ctx.func_stack_ptr += TYPE_SIZE;
			this->memory->stack_ptr = ctx.func_stack_ptr;
            out << "\t push rax" << "\n";

			break;
		}
		case lexing::token_type::MINUS: {
			load_rax_rcx_arithmetic();
			out << "\t sub rax, rcx\n";

            const size_t TYPE_SIZE = ctx.type_system.all_types[this->memory->type_ind]->size;
			ctx.func_stack_ptr += TYPE_SIZE;
			this->memory->stack_ptr = ctx.func_stack_ptr;
            out << "\t push rax" << "\n";

			break;
		}
		case lexing::token_type::STAR: {
			load_rax_rcx_arithmetic();
			out << "\t mul rcx\n";

            const size_t TYPE_SIZE = ctx.type_system.all_types[this->memory->type_ind]->size;
			ctx.func_stack_ptr += TYPE_SIZE;
			this->memory->stack_ptr = ctx.func_stack_ptr;
            out << "\t push rax" << "\n";

			break;
		}
		case lexing::token_type::SLASH: {
			load_rax_rcx_arithmetic();
			out << "\t div rcx\n";

            const size_t TYPE_SIZE = ctx.type_system.all_types[this->memory->type_ind]->size;
			ctx.func_stack_ptr += TYPE_SIZE;
			this->memory->stack_ptr = ctx.func_stack_ptr;
            out << "\t push rax" << "\n";

			break;
		}
		case lexing::token_type::MODULO: {
			load_rax_rcx_arithmetic();
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
