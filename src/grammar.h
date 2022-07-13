#pragma once

#include <string_view>
#include <memory>
#include <iostream>

namespace grammar {

struct ast_node;
struct expression;
struct statement;
struct number_literal;
struct identifier_literal;
struct binary_expression;
struct expression_statement;
struct list_statement;
struct let_statement;
struct return_statement;
struct global_declaration;
struct function_declaration;
struct program;

};

#include "lexing.h"
#include "parsing.h"
#include "typing.h"

// Abstract classes are here

namespace grammar {

struct ast_node {
    ast_node() = default;
    virtual ~ast_node() = default;

	virtual void print(std::ostream &out, const size_t identation) const = 0;
	virtual void emit_code(std::ostream &out, parsing::context &ctx) = 0;
};

struct expression : public ast_node {
    size_t type;
	size_t stack_ptr;

    expression() : type(typing::NOT_INFERED_ID), stack_ptr((size_t)-1) {};
    virtual ~expression() = default;

    virtual void try_infering_type(parsing::context &context) = 0;
};

struct statement : public ast_node {
    statement() = default;
    virtual ~statement() = default;
};

struct global_declaration : public ast_node {
    global_declaration() = default;
    virtual ~global_declaration() = default;

    virtual void print(std::ostream &out, const size_t identation) const = 0;
};

};

#include "ast/number_literal.h"
#include "ast/identifier_literal.h"
#include "ast/binary_expression.h"
#include "ast/expression_statement.h"
#include "ast/list_statement.h"
#include "ast/let_statement.h"
#include "ast/return_statement.h"
#include "ast/function_declaration.h"
#include "ast/program.h"