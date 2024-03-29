#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <string_view>
#include <optional>

namespace typing {

const size_t NOT_INFERED_ID = 0;
const size_t VOID_ID = 1;
const size_t I32_ID = 2;
const size_t I64_ID = 3;
const size_t BOOL_ID = 4;

struct simple_type {
	const std::string name;
	const size_t size;

    simple_type(const std::string &name, const size_t size);
    virtual ~simple_type() = default;
};

struct type_system {
    std::vector<std::shared_ptr<simple_type> > all_types;
    std::map<std::string, size_t> type_map;

    type_system();
    ~type_system() = default;

    std::pair<bool, size_t> add_or_get_type(const std::shared_ptr<simple_type> &type);
    std::optional<size_t> find_type(const std::string &name) const;
};

};
