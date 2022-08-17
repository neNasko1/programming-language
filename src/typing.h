#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <string_view>
#include <optional>

namespace typing {

const size_t NOT_INFERED_ID = 0;
const size_t VOID_ID = 2;
const size_t I32_ID = 4;
const size_t I64_ID = 6;
const size_t BOOL_ID = 8;

struct type_system;

struct simple_type {
	const type_system *parent_system;

    const std::string name;
	const size_t size;

    simple_type(const std::string &name, const size_t size, const type_system *parent_system = nullptr);
    virtual ~simple_type() = default;
};

struct reference_type : public simple_type {
    const size_t ref_to;

    reference_type(const std::string &ref_name, const size_t ref_to, const type_system *parent_system = nullptr);
    ~reference_type() = default;
};

struct type_system {
    std::vector<std::shared_ptr<simple_type> > all_types;
    std::map<std::string, size_t> type_map;

    type_system();
    ~type_system() = default;

    std::pair<bool, size_t> add_simple_type(const std::string &name, const size_t size);
    std::optional<size_t> find_type(const std::string &name) const;
};

};
