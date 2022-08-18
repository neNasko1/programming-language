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

reference_type::reference_type(const std::string &ref_name, const size_t ref_to, const type_system *parent_system) : simple_type(ref_name, 8, parent_system), ref_to(ref_to) {}

// Assume that indexing is correct, TODO: learn how to use c++
type_system::type_system() {
    // Adding this types should always result in ids 0, 1 ...

    #define mk_st(name, size) std::make_shared<simple_type>(name, size)

    assert(this->add_type(mk_st("not_infered", 0)).second == NOT_INFERED_ID);
    assert(this->add_type(mk_st("void", 0)).second == VOID_ID);
    assert(this->add_type(mk_st("i32", 4)).second == I32_ID);
    assert(this->add_type(mk_st("i64", 8)).second == I64_ID);
    assert(this->add_type(mk_st("bool", 1)).second == BOOL_ID);

    this->add_type(mk_st("i8", 1));
    this->add_type(mk_st("i16", 2));

    #undef mk_st
}

std::pair<bool, size_t> type_system::add_type(const std::shared_ptr<simple_type> &new_type) {
    const auto res = this->type_map.find(new_type->name);

    if(res != this->type_map.end()) {
        return {false, res->second};
    } else {
        const size_t ind = this->all_types.size();
        this->type_map[new_type->name] = ind;
        this->all_types.push_back(std::move(new_type));

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
