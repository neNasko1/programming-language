#pragma once

#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"

namespace grammar {

struct list_statement : public statement {
    const std::vector<std::unique_ptr<statement> > body;

    list_statement() = default;
    list_statement(std::vector<std::unique_ptr<statement> > &body);
    ~list_statement() = default;

    void print(std::ostream &out, const size_t identation) const;
	void emit_code(std::ostream &out, parsing::context &ctx);
};

};