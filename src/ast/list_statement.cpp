#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "list_statement.h"

namespace grammar {

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
	const auto snapshot = ctx.func_stack_ptr;

	for(const auto &stat : this->body) {
		stat->emit_code(out, ctx);
	}

	// TODO: Random cheat for inserting distinct labels
	const std::string name = "statement_list_clearup" + std::to_string((uint64_t)this);

	out << name << ":\n";

	out << "\t add rsp, " << ctx.func_stack_ptr - snapshot << "\n";
	ctx.func_stack_ptr = snapshot;
}

};
