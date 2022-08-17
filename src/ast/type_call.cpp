#include <string_view>
#include <memory>
#include <string>
#include <cassert>

#include "../grammar.h"

namespace grammar {

type_call::type_call(const std::string &name) : name(name), args(), internal_name(name) {}

type_call::type_call(const std::string &name, std::vector<std::unique_ptr<type_call> > &args) : name(name), args(std::move(args)), internal_name(name) {
    bool first = true;
    for(const auto &it : this->args) {
        if(first) {
            this->internal_name += '<';
            first = false;
        } else {
            this->internal_name += ',';
        }
        this->internal_name += it->internal_name;
    }
    if(!this->args.empty()) {
        this->internal_name += '>';
    }
}

void type_call::print(std::ostream &out, const size_t ident) const {
    const std::string tabulation = std::string(ident, '\t');

    out << tabulation << this->internal_name << std::endl;
}

void compile(std::ostream &out, parsing::context &ctx) {
    assert(false);
}

};