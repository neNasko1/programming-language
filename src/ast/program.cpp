#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "program.h"

namespace grammar {

program::program(std::vector<std::unique_ptr<function_declaration> > &function_declarations) : function_declarations(std::move(function_declarations)) {}

void program::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "program(" << std::endl;
    for(const auto &it : this->function_declarations) {
        it->print(out, ident + 1);
    }
    out << tabulation << ")" << std::endl;
}

void program::emit_code(std::ostream &out, parsing::context &ctx) {
	out << "\t section .text\n";
    for(const auto &decl : this->function_declarations) {
        ctx.declare_function(decl->name, decl.get());
        decl->try_infering_type(ctx);
    }

	for(const auto &decl : this->function_declarations) {
		ctx.current_declaration = decl.get();
        ctx.variables.clear();

		decl->emit_code(out, ctx);
		ctx.func_stack_ptr = 0;
	}
}

};
