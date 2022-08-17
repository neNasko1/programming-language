#pragma once

#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"

namespace grammar {

struct let_statement : public statement {
    const std::string name;
    const std::string type_hint;
    const std::unique_ptr<expression> init_value;

    std::unique_ptr<memory_cell> memory;

    let_statement(const std::string &name, std::unique_ptr<expression> init_value);
    let_statement(const std::string &name, const std::string &type_hint, std::unique_ptr<expression> init_value);
    ~let_statement() = default;

    void print(std::ostream &out, const size_t identation) const;
	void compile(std::ostream &out, parsing::context &ctx);
};

};