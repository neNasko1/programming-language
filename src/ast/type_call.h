#pragma once

#include <string_view>
#include <memory>
#include <string>
#include <cassert>

#include "../grammar.h"

namespace grammar {

struct type_call : public ast_node {
    const std::string name;
    std::vector<std::unique_ptr<type_call> > args;

    std::string internal_name;

    type_call(const std::string &name);
    type_call(const std::string &name, std::vector<std::unique_ptr<type_call> > &args);
    ~type_call() = default;

    void print(std::ostream &out, const size_t identation) const;
	void compile(std::ostream &out, parsing::context &ctx);
};

};