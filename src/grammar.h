#pragma once

#include <string_view>
#include <memory>
#include <iostream>

#include "lexing.h"

namespace grammar {

struct ast_node {
    ast_node() = default;
    virtual ~ast_node() = default;
    virtual void print(std::ostream &out, const size_t identation) const = 0;
};

struct expression : public ast_node {
    expression() = default;
    virtual ~expression() = default;
};

struct statement : public ast_node {
    statement() = default;
    virtual ~statement() = default;
};

struct number_literal : public expression {
    const std::string_view value;

    number_literal(const std::string_view value);
    ~number_literal() = default;
    void print(std::ostream &out, const size_t identation) const;
};

struct identifier_literal : public expression {
    const std::string_view value;

    identifier_literal(const std::string_view value);
    ~identifier_literal() = default;
    
    void print(std::ostream &out, const size_t identation) const;
};

struct binary_expression : public expression {
    const std::unique_ptr<expression> lft;
    const lexing::token op;    
    const std::unique_ptr<expression> rght;

    binary_expression(std::unique_ptr<expression> lft, const lexing::token &op, std::unique_ptr<expression> rght);
    ~binary_expression() = default;
    
    void print(std::ostream &out, const size_t identation) const;
};

struct expression_statement : public statement {
    const std::unique_ptr<expression> expr;

    expression_statement(std::unique_ptr<expression> expr);
    ~expression_statement() = default;

    void print(std::ostream &out, const size_t identation) const;
};

struct list_statement : public statement {
    std::vector<std::unique_ptr<statement> > body;

    list_statement() = default;
    list_statement(std::vector<std::unique_ptr<statement> > &body);
    ~list_statement() = default;

    void print(std::ostream &out, const size_t identation) const;
};

struct let_statement : public statement {
    const std::string_view name;
    const std::string_view type;
    const std::unique_ptr<expression> init_value;

    let_statement(const std::string_view name, std::unique_ptr<expression> init_value);
    let_statement(const std::string_view name, const std::string_view type, std::unique_ptr<expression> init_value);
    ~let_statement() = default;   

    void print(std::ostream &out, const size_t identation) const;
};

struct return_statement : public statement {
    const std::unique_ptr<expression> value;

    return_statement(std::unique_ptr<expression> value);
    ~return_statement() = default;   

    void print(std::ostream &out, const size_t identation) const;
};

struct global_declaration : public ast_node {
    global_declaration() = default;
    virtual ~global_declaration() = default; 

    virtual void print(std::ostream &out, const size_t identation) const = 0;
};

struct function_declaration : public global_declaration {
    const std::string_view name;
    const std::string_view type;
    const std::unique_ptr<statement> body;

    function_declaration(const std::string_view name, const std::string_view type, std::unique_ptr<statement> body);
    ~function_declaration() = default;

    void print(std::ostream &out, const size_t identation) const;
};

struct program : public ast_node {
    std::vector<std::unique_ptr<global_declaration> > definitions;

    program() = default;
    ~program() = default;
    void print(std::ostream &out, const size_t identation) const;
};

};