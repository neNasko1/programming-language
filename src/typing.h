#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <string_view>

namespace typing {

const size_t NOT_INFERED_ID = 0;
const size_t VOID_ID = 1;
const size_t I32_ID = 2;

// TODO: Add hashing for fast type lookup
struct string_comparator {
    const std::string value;

    string_comparator(const std::string &value);
    string_comparator(const std::string_view &value);
    ~string_comparator() = default;

    bool operator <(const string_comparator &other) const;
};

struct type {
	size_t size;

    type(const size_t size);
    virtual ~type() = default;

    virtual const string_comparator &get_comp() const = 0;
	virtual size_t get_size() const = 0;
};

struct simple_type : public type {
    const std::string name;
    const string_comparator comp;

    simple_type(const char *name, const size_t size);
    simple_type(const std::string &name, const size_t size);
    ~simple_type() = default;

    const string_comparator &get_comp() const;
	size_t get_size() const;
};

struct type_system {
    std::vector<std::shared_ptr<type> > all_types;
    std::map<string_comparator, size_t> type_map;

    type_system();
    ~type_system() = default;

    std::pair<bool, size_t> add_or_get_type(const std::shared_ptr<type> &type);
    std::pair<bool, size_t> find_type(const string_comparator &comp) const;
};

};
