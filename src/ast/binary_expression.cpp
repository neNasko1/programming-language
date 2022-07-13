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

	out << "; binary_expression " << lexing::reverse_token_type_names[this->op.type] << std::endl;

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
		default: {
			assert(false);
		}
	}

    out << std::endl;
}


};