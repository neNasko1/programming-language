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
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "return " << std::endl;
    this->value->print(out, ident + 1);
}

void return_statement::emit_code(std::ostream &out, parsing::context &ctx) {
	this->value->emit_code(out, ctx);

	if(ctx.current_declaration->name == "_start") {
		out << "mov rdi, " << "[rsp+" << ctx.func_stack_ptr - this->value->stack_ptr << "]\n";
		out << "mov rax, 60\n";
		out << "syscall\n";
	} else {
        assert(false);
    }
}

};