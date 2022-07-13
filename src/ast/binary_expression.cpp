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

	assert(this->lft->memory->type == this->rght->memory->type);
	this->memory->type = this->lft->memory->type;
}

void binary_expression::emit_code(std::ostream &out, parsing::context &ctx) {
	this->try_infering_type(ctx);
	assert(this->memory->type != typing::NOT_INFERED_ID);

	assert(this->memory->type == typing::I64_ID); // TODO: Handle different types of expressions

	this->lft->emit_code(out, ctx);
	this->rght->emit_code(out, ctx);

	out << "\t ; binary_expression " << lexing::reverse_token_type_names[this->op.type] << std::endl;

	switch(this->op.type) {
		case lexing::token_type::PLUS: {
			out << "\t mov " << " rax, " << "[rsp+" << ctx.func_stack_ptr - this->lft->memory->stack_ptr << "]\n";
			out << "\t add " << " rax, " << "[rsp+" << ctx.func_stack_ptr - this->rght->memory->stack_ptr << "]\n";

            const size_t TYPE_SIZE = ctx.type_system.all_types[this->memory->type]->size;
			ctx.func_stack_ptr += TYPE_SIZE;
			this->memory->stack_ptr = ctx.func_stack_ptr;
            out << "\t sub rsp," << TYPE_SIZE << "\n";
            out << "\t mov [rsp], rax\n"; // Push the value to the stack

			break;
		}
		default: {
			assert(false);
		}
	}

    out << std::endl;
}


};