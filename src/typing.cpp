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

simple_type::simple_type(const std::string &name, const size_t size) : name(name), size(size) {}

// Assume that indexing is correct, TODO: learn how to use c++
type_system::type_system() {
    // Adding this types should always result in ids 0, 1 ...
    assert(this->add_or_get_type(std::make_shared<simple_type>("not_infered", 0)).second == NOT_INFERED_ID);
    assert(this->add_or_get_type(std::make_shared<simple_type>("void", 0)).second == VOID_ID);
    assert(this->add_or_get_type(std::make_shared<simple_type>("i32", 4)).second == I32_ID);
    assert(this->add_or_get_type(std::make_shared<simple_type>("i64", 8)).second == I64_ID);
    assert(this->add_or_get_type(std::make_shared<simple_type>("bool", 1)).second == BOOL_ID);

    this->add_or_get_type(std::make_shared<simple_type>("i8", 1));
    this->add_or_get_type(std::make_shared<simple_type>("i16", 2));
}

std::pair<bool, size_t> type_system::add_or_get_type(const std::shared_ptr<simple_type> &type) {
    const auto res = this->type_map.find(type->name);
    if(res != this->type_map.end()) {
        return {false, res->second};
    } else {
        this->all_types.push_back(type);
        this->type_map[type->name] = this->all_types.size() - 1;

        return {true, this->all_types.size() - 1};
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
