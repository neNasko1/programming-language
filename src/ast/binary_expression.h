#pragma once

#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"

namespace grammar {

struct binary_expression : public expression {
    const std::unique_ptr<expression> lft;
    const lexing::token op;
    const std::unique_ptr<expression> rght;

    binary_expression(std::unique_ptr<expression> lft, const lexing::token &op, std::unique_ptr<expression> rght);
    ~binary_expression() = default;

    void print(std::ostream &out, const size_t identation) const;
    void try_infering_type(parsing::context &context);
	void emit_code(std::ostream &out, parsing::context &ctx);
};

};