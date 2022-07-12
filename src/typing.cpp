#include <string>
#include <map>
#include <vector>
#include <memory>
#include <assert.h>
#include <string_view>
#include <iostream>

#include "typing.h"

namespace typing {

string_comparator::string_comparator(const std::string &value) : value(value) { }
string_comparator::string_comparator(const std::string_view &value) : value(std::string(value)) { }
bool string_comparator::operator <(const string_comparator &other) const {
	return this->value < other.value;
}

type::type(const size_t size) : size(size) {}

simple_type::simple_type(const char *name, const size_t size) : type(size), name(name), comp(std::string(name)) {}
simple_type::simple_type(const std::string &name, const size_t size) : type(size), name(name), comp(name) {}

const string_comparator &simple_type::get_comp() const {
    return this->comp;
}

size_t simple_type::get_size() const {
	return this->size;
}

// Assume that indexing is correct, TODO: learn how to use c++
type_system::type_system() {
    // Adding this types should always result in ids 0, 1 ...
	std::cerr << NOT_INFERED_ID << " " << VOID_ID << std::endl;
    assert(this->add_or_get_type(std::make_shared<simple_type>("not_infered", 0)).second == NOT_INFERED_ID);
    assert(this->add_or_get_type(std::make_shared<simple_type>("void", 0)).second == VOID_ID);
    assert(this->add_or_get_type(std::make_shared<simple_type>("i32", 4)).second == I32_ID);
    assert(this->add_or_get_type(std::make_shared<simple_type>("i64", 8)).second == I64_ID);

    this->add_or_get_type(std::make_shared<simple_type>("i8" , 1));
    this->add_or_get_type(std::make_shared<simple_type>("i16", 2));
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
