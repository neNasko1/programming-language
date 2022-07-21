#pragma once

#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"

namespace grammar {

struct if_statement : public statement {
    const std::unique_ptr<expression> cond;
	const std::unique_ptr<statement> body;

    if_statement(std::unique_ptr<expression> cond, std::unique_ptr<statement> body);
    ~if_statement() = default;

    void print(std::ostream &out, const size_t identation) const;
	void emit_code(std::ostream &out, parsing::context &ctx);
};

};
