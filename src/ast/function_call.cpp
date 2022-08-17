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
    if(this->memory->type != typing::NOT_INFERED_ID) { return; }

    const auto res = ctx.get_function_definition(this->name);
    assert(res);

    this->definition = res.value();
	this->memory->type = this->definition->type;
}

void function_call::compile(std::ostream &out, parsing::context &ctx) {
    this->try_infering_type(ctx);

    out << "\t ; function call " << this->name << "\n";

    assert(this->args.size() == this->definition->params.size());
    for(size_t i = 0; i < this->args.size(); i ++) {
		this->args[i]->compile(out, ctx);
        assert(this->args[i]->memory->type == this->definition->params[i]->memory->type);
    }

	// Alocate space for the return value
	const size_t TYPE_SIZE = ctx.type_system.all_types[this->memory->type]->size;
	ctx.func_stack_ptr += TYPE_SIZE;
	this->memory->stack_ptr = ctx.func_stack_ptr;
	out << "\t sub rsp, " << TYPE_SIZE << "\n";

	// We need to transfer params in reverse order
    for(int i = this->args.size() - 1; i >= 0; i --) {
        const auto mem = this->args[i]->memory.get();

		const size_t PARAM_TYPE_SIZE = ctx.type_system.all_types[mem->type]->size;
		asm_helper::push_to_stack(out, ctx.func_stack_ptr - mem->stack_ptr, PARAM_TYPE_SIZE);
		ctx.func_stack_ptr += PARAM_TYPE_SIZE;
	}

    out << "\t call " << this->name << "\n";

	out << "\t add rsp, " << this->definition->args_size << "\n\n";
	ctx.func_stack_ptr -= this->definition->args_size;

    out << std::endl;
}

};
