#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "if_statement.h"

namespace grammar {

if_statement::if_statement(std::unique_ptr<expression> cond, std::unique_ptr<statement> body) : cond(std::move(cond)), body(std::move(body)) {};

void if_statement::print(std::ostream &out, const size_t ident) const {
    const std::string tabulation = std::string(ident, '\t');

    out << tabulation << "if statement (" << std::endl;
    this->cond->print(out, ident + 1);
    out << tabulation << ")" << std::endl;
	this->body->print(out, ident + 1);
}

void if_statement::compile(std::ostream &out, parsing::context &ctx) {
	this->cond->compile(out, ctx);
	assert(this->cond->memory->type == typing::BOOL_ID);

	out << "\t mov al, " << "[rsp+" << ctx.func_stack_ptr - this->cond->memory->stack_ptr << "]\n";
	out << "\t cmp al, 0\n";

	// TODO: Random cheat for inserting distinct labels
	const std::string name = "if_label_not" + std::to_string((uint64_t)this);
	out << "\t je " << name << "\n";
	this->body->compile(out, ctx);
	out << name << ":\n";
}

};
