#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"
#include "function_declaration.h"

namespace grammar {

function_declaration::function_declaration(const std::string_view name, const std::string_view type, std::unique_ptr<statement> body)
    : name(name != "main" ? name : "_start"), type(type), body(std::move(body)) {
}

void function_declaration::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "function " << this->name << " : " << this->type << std::endl;
    this->body->print(out, ident);
}

void function_declaration::emit_code(std::ostream &out, parsing::context &ctx) {
	out << "global " << this->name << "\n";
	out << this->name << ":\n";

	this->body->emit_code(out, ctx);
}

};