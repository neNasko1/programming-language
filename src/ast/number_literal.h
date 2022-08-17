#pragma once

#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"

namespace grammar {

struct number_literal : public expression {
    const std::string value;

    number_literal(const std::string &value);
    ~number_literal() = default;

    void print(std::ostream &out, const size_t identation) const;
    void try_infering_type(parsing::context &context);
	void emit_code(std::ostream &out, parsing::context &ctx);
};

};