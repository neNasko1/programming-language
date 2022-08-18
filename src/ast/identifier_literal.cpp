#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "identifier_literal.h"

namespace grammar {

identifier_literal::identifier_literal(const std::string &name) : name(name) {}

void identifier_literal::print(std::ostream &out, const size_t ident) const {
    const std::string tabulation = std::string(ident, '\t');

    out << tabulation << "identifier(" << this->name << ")" << std::endl;
}

void identifier_literal::try_infering_type(parsing::context &ctx) {
    if(this->memory->type_ind != typing::NOT_INFERED_ID) { return; }

	const auto res = ctx.get_variable_definition(this->name);
	assert(res);

	const auto ref_name = "&<" + ctx.type_system.all_types[res.value()->type_ind]->name + ">";
	auto found_type = ctx.type_system.find_type(ref_name);

	if(!found_type) {
		found_type = ctx.type_system.add_type(
				std::make_shared<typing::reference_type>(ref_name, res.value()->type_ind, &(ctx.type_system))
			).second;
	}

	this->memory->type_ind = found_type.value();
}

void identifier_literal::compile(std::ostream &out, parsing::context &ctx) {
    this->try_infering_type(ctx);

	// TODO: Remove the second query
	const auto res = ctx.get_variable_definition(this->name);
	assert(res);

	out << "\t lea rax, " << "[rsp+" << ctx.func_stack_ptr - res.value()->stack_ptr << "]\n";
    out << "\t push rax " << "\n";

    const size_t REF_SIZE = ctx.type_system.all_types[this->memory->type_ind]->size;
	ctx.func_stack_ptr += REF_SIZE;
	this->memory->stack_ptr = ctx.func_stack_ptr;

    out << std::endl;
}

};
