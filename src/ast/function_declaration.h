#pragma once

#include <string_view>
#include <memory>
#include <iostream>

#include "../grammar.h"
#include "../lexing.h"
#include "../parsing.h"

namespace grammar {

struct function_declaration : public global_declaration {
    const std::string_view name;
    const std::string_view type_hint;
    const std::unique_ptr<statement> body;
    const std::vector<std::unique_ptr<function_declaration_parameter> > params;

    size_t type;
    size_t return_size;
    size_t args_size;

    function_declaration(const std::string_view name, const std::string_view type_hint, std::unique_ptr<statement> body, std::vector<std::unique_ptr<function_declaration_parameter> > &params);
    function_declaration(const std::string_view name, const std::string_view type_hint, std::vector<std::unique_ptr<function_declaration_parameter> > &params);
    ~function_declaration() = default;

    void print(std::ostream &out, const size_t identation) const;
    void try_infering_type(parsing::context &ctx);
	void emit_code(std::ostream &out, parsing::context &ctx);
};

};
