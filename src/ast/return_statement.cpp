#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "return_statement.h"

namespace grammar {

return_statement::return_statement(std::unique_ptr<expression> value) : value(std::move(value)) {}

void return_statement::print(std::ostream &out, const size_t ident) const {
    const std::string tabulation = std::string(ident, '\t');

    out << tabulation << "return " << std::endl;
    this->value->print(out, ident + 1);
}

void return_statement::compile(std::ostream &out, parsing::context &ctx) {
	this->value->compile(out, ctx);

	const auto val_tid = this->value->memory->type_ind;
	const auto val_uref = ctx.type_system.unwrap_ref(val_tid);

	const auto ret_tid = ctx.current_declaration->type_ind;
	const auto ret_uref = ctx.type_system.unwrap_ref(ret_tid);

	assert(val_uref.first == ret_uref.first && val_uref.second >= ret_uref.second);

	const auto move_up = val_uref.second - ret_uref.second;

	if(move_up) {
		out << "\t lea rcx, [rsp+" << ctx.func_stack_ptr - this->value->memory->stack_ptr << "]\n";
		for(size_t i = 0; i < move_up; i ++) {
			out << "\t mov rcx, [rcx]\n";
		}
		out << "\t mov rax, [rcx]\n";
	} else {
		out << "\t mov rax, [rsp+" << ctx.func_stack_ptr - this->value->memory->stack_ptr << "]\n";
	}

	if(ctx.current_declaration->name == "main") {
		out << "\t mov rdi, rax\n";
        out << "\t jmp _cleanup_" << ctx.current_declaration->name << "\n";
	} else {
		// TODO: remove
		assert(ctx.type_system.all_types[ctx.current_declaration->type_ind]->size == 8);

		out << "\t mov [rsp + "
			<< ctx.func_stack_ptr - ctx.type_system.all_types[ret_tid]->size << "], rax\n";
        out << "\t jmp _cleanup_" << ctx.current_declaration->name << "\n";
    }

    out << std::endl;
}

};
