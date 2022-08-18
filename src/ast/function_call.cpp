#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "../asm_helper.h"
#include "function_call.h"

namespace grammar {

function_call::function_call(const std::string &name, std::vector<std::unique_ptr<expression> > args) : name(name), args(std::move(args)) {}

void function_call::print(std::ostream &out, const size_t ident) const {
    const std::string tabulation = std::string(ident, '\t');

    out << tabulation << "function call " << this->name << " (" << std::endl;
    for(const auto &arg : this->args) {
        arg->print(out, ident + 1);
        out << tabulation <<  "," << std::endl;
    }
    out << tabulation << ")" << std::endl;

}

void function_call::try_infering_type(parsing::context &ctx) {
    if(this->memory->type_ind != typing::NOT_INFERED_ID) { return; }

    const auto res = ctx.get_function_definition(this->name);
    assert(res);

    this->definition = res.value();
	this->memory->type_ind = this->definition->type_ind;
}

void function_call::compile(std::ostream &out, parsing::context &ctx) {
    this->try_infering_type(ctx);

    out << "\t ; function call " << this->name << "\n";

    assert(this->args.size() == this->definition->params.size());
    for(size_t i = 0; i < this->args.size(); i ++) {
		this->args[i]->compile(out, ctx);

        const auto mem = this->args[i]->memory.get();
		const auto arg_tid = mem->type_ind;
		const auto arg_uref = ctx.type_system.unwrap_ref(arg_tid);

		const auto req_arg_tid = this->definition->params[i]->memory->type_ind;
		const auto req_arg_uref = ctx.type_system.unwrap_ref(req_arg_tid);
		assert(arg_uref.first == req_arg_uref.first && arg_uref.second >= req_arg_uref.second);
    }

	// Alocate space for the return value
	const size_t TYPE_SIZE = ctx.type_system.all_types[this->memory->type_ind]->size;
	ctx.func_stack_ptr += TYPE_SIZE;
	this->memory->stack_ptr = ctx.func_stack_ptr;
	out << "\t sub rsp, " << TYPE_SIZE << "\n";

	// We need to transfer params in reverse order
    for(int i = this->args.size() - 1; i >= 0; i --) {
        const auto mem = this->args[i]->memory.get();
		const auto arg_tid = mem->type_ind;
		const auto arg_uref = ctx.type_system.unwrap_ref(arg_tid);

		const auto req_arg_tid = this->definition->params[i]->memory->type_ind;
		const auto req_arg_uref = ctx.type_system.unwrap_ref(req_arg_tid);

		const auto move_up_cnt = arg_uref.second - req_arg_uref.second;

		if(move_up_cnt) {
			out << "\t lea rcx, [rsp+" << ctx.func_stack_ptr - this->args[i]->memory->stack_ptr << "]\n";
			for(size_t i = 0; i < move_up_cnt - 1; i ++) {
				out << "\t mov rcx, [rcx]\n";
			}
			out << "\t mov rax, [rcx]\n";
		} else {
			out << "\t lea rax, [rsp+" << ctx.func_stack_ptr - this->args[i]->memory->stack_ptr << "]\n";
		}

		const size_t PARAM_TYPE_SIZE = ctx.type_system.all_types[arg_uref.first]->size;
		asm_helper::push_to_stack_str(out, std::string("rax"), PARAM_TYPE_SIZE);
		ctx.func_stack_ptr += PARAM_TYPE_SIZE;
	}

    out << "\t call " << this->name << "\n";

	out << "\t add rsp, " << this->definition->args_size << "\n\n";
	ctx.func_stack_ptr -= this->definition->args_size;

    out << std::endl;
}

};
