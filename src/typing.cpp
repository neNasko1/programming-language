#include <string>
#include <map>
#include <vector>
#include <memory>
#include <assert.h>

#include "typing.h"

namespace typing {

string_comparator::string_comparator(const std::string &value) : value(value) {}
bool string_comparator::operator <(const string_comparator &other) const {
    return this->value < other.value;
}

simple_type::simple_type(const char *name) : name(name), comp(std::string(name)) {}
simple_type::simple_type(const std::string &name) : name(name), comp(name) {}
const string_comparator &simple_type::get_comp() const {
    return this->comp;
}

// Assume that indexing is correct, TODO: learn how to use c++
type_system::type_system() {
    assert(this->add_or_get_type(std::make_shared<simple_type>("not_infered")).second == NOT_INFERED_ID); // Assert that the 
    assert(this->add_or_get_type(std::make_shared<simple_type>("void")).second == VOID_ID);
    this->add_or_get_type(std::make_shared<simple_type>("i32"));
    this->add_or_get_type(std::make_shared<simple_type>("i64"));
}

std::pair<bool, size_t> type_system::add_or_get_type(const std::shared_ptr<type> &type) {
    const auto res = this->type_map.find(type->get_comp());
    if(res != this->type_map.end()) {
        return {false, res->second};
    } else {
        this->all_types.push_back(type);
        this->type_map[type->get_comp()] = this->all_types.size() - 1;

        return {true, this->all_types.size() - 1};
    }

    assert(false);
}

std::pair<bool, size_t> type_system::find_type(const string_comparator &comp) const {
    const auto res = this->type_map.find(comp);
    if(res != this->type_map.end()) {
        return {true, res->second};
    } else {
        return {false, NOT_INFERED_ID};
    }

    assert(false);
}

};