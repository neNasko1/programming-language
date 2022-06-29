#pragma once

#include <vector>
#include <memory>
#include <cassert>
#include <stack>

#include "lexing.h"
#include "grammar.h"

namespace parsing {

struct parser {
    std::string code; // The parser will own the code, as we need it for all the string_view-s
    std::vector<lexing::token> tokens;

    size_t pit; // Position in tokens

    parser(lexing::lexer &lex);
    ~parser() = default;

    lexing::token peek() const;
    lexing::token advance();
    bool match(const lexing::token_type type);
    bool is_at_end() const;

    std::unique_ptr<grammar::expression> parse_expression();
    std::unique_ptr<grammar::expression_statement> parse_expression_statement();
    std::unique_ptr<grammar::list_statement> parse_list_statement();
    std::unique_ptr<grammar::let_statement> parse_let_statement();
    std::unique_ptr<grammar::return_statement> parse_return_statement();
    std::unique_ptr<grammar::function_declaration> parse_function_declaration();
    std::unique_ptr<grammar::global_declaration> parse_definition();
    std::unique_ptr<grammar::program> parse_program();
};

#undef HARD_MATCH

};