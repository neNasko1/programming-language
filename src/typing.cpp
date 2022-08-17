#include <string>
#include <map>
#include <vector>
#include <memory>
#include <assert.h>
#include <string_view>
#include <iostream>
#include <optional>

#include "typing.h"

namespace typing {

simple_type::simple_type(const std::string &name, const size_t size, const type_system *parent_system) : name(name), size(size), parent_system(parent_system) {}

reference_type::reference_type(const std::string &ref_name, const size_t ref_to, const type_system *parent_system) : simple_type(ref_name + "&", 8, parent_system), ref_to(ref_to) {}

// Assume that indexing is correct, TODO: learn how to use c++
type_system::type_system() {
    // Adding this types should always result in ids 0, 1 ...
    assert(this->add_simple_type("not_infered", 0).second == NOT_INFERED_ID);
    assert(this->add_simple_type("void", 0).second == VOID_ID);
    assert(this->add_simple_type("i32", 4).second == I32_ID);
    assert(this->add_simple_type("i64", 8).second == I64_ID);
    assert(this->add_simple_type("bool", 1).second == BOOL_ID);

    this->add_simple_type("i8", 1);
    this->add_simple_type("i16", 2);
}

std::pair<bool, size_t> type_system::add_simple_type(const std::string &name, const size_t size) {
    const auto res = this->type_map.find(name);

    if(res != this->type_map.end()) {
        return {false, res->second};
    } else {
        const size_t ind = this->all_types.size();
        this->all_types.push_back(std::make_unique<simple_type>(name, size, this));
        this->type_map[name] = ind;

        this->all_types.push_back(std::make_shared<reference_type>(name, ind, this));
        this->type_map[this->all_types.back()->name] = ind;

        return {true, ind};
    }

    assert(false);
}

std::optional<size_t> type_system::find_type(const std::string &comp) const {
    const auto res = this->type_map.find(comp);
    if(res != this->type_map.end()) {
        return std::make_optional(res->second);
    } else {
        return std::nullopt;
    }

    assert(false);
}

};
