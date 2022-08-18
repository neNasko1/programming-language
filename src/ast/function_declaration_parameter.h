#pragma once

#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"

namespace grammar {

struct function_declaration_parameter : public ast_node {
    const std::string name;
    const std::unique_ptr<type_call> type;

    std::unique_ptr<memory_cell> memory;

    function_declaration_parameter(const std::string &name, std::unique_ptr<type_call> &type);
    ~function_declaration_parameter() = default;

    void print(std::ostream &out, const size_t identation) const;
    void compile(std::ostream &out, parsing::context &ctx);
};

};
