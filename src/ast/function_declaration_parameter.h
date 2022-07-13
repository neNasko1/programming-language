#pragma once

#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"

namespace grammar {

struct function_declaration_parameter : public ast_node {
    const std::string_view name;
    const std::string_view type_hint;

    size_t type;

    std::unique_ptr<memory_cell> memory;

    function_declaration_parameter(const std::string_view name, const std::string_view type_hint);
    ~function_declaration_parameter() = default;

    void print(std::ostream &out, const size_t identation) const;
    void emit_code(parsing::context &context);
};

};
