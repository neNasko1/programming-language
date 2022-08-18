#include <vector>
#include <memory>

#include "lexing.h"
#include "parsing.h"
#include "grammar.h"

namespace parsing {

parser::parser(lexing::lexer &lex) : code(std::move(lex.code)), tokens(std::move(lex.tokens)), pit(0) {}

lexing::token parser::peek() const {
    return this->tokens[this->pit];
}

lexing::token parser::advance() {
    return this->tokens[this->pit ++];
}

bool parser::match(const lexing::token_type type) {
    return (this->peek().type == type) ? this->pit ++, true : false;
}

bool parser::is_at_end() const {
    return this->pit == this->tokens.size();
}

std::unique_ptr<grammar::type_call> parser::parse_type_call() {
    std::string name = "";
    std::vector<std::unique_ptr<grammar::type_call> > args = {};

    if(this->peek().type == lexing::token_type::IDENTIFIER) {
        name = this->peek().value;
        this->advance();

        if(this->match(lexing::token_type::AND)) {
            args.push_back(std::make_unique<grammar::type_call>(name));
            name = "&";
        }
    } else {
        name = "";
    }

	if(name.size() == 0) {
		return nullptr;
	} else {
    	return std::make_unique<grammar::type_call>(name, args);
	}
}

std::unique_ptr<grammar::expression> parser::parse_function_call() {
    const auto name = this->peek().value;
    assert(this->match(lexing::token_type::IDENTIFIER));
    assert(this->match(lexing::token_type::L_PAREN));

    std::vector<std::unique_ptr<grammar::expression> > args;

    while(!this->match(lexing::token_type::R_PAREN)) {
        args.push_back(std::move(this->parse_expression()));

        if(!this->match(lexing::token_type::COMMA)) {
            assert(this->match(lexing::token_type::R_PAREN));
            break;
        }
    }

    return std::make_unique<grammar::function_call>(name, std::move(args));
}

// Copied from https://github.com/neNasko1/fake-script/blob/main/src/grammar.cpp
std::unique_ptr<grammar::expression> parser::parse_expression() {
    std::stack<std::unique_ptr<grammar::expression> > expression_stack;
    std::stack<lexing::token> operation_stack;
    bool can_be_unary = true;
    int cnt_lparen = 0;

    const auto combine_top = [&]() {
        assert(!operation_stack.empty());

        const auto op = operation_stack.top(); operation_stack.pop();
        if(lexing::utils::op::is_unary(op)) {
            assert(false);
            /*
            if(expression_stack.empty()) {
                std::cerr << "There are no expressions in the expression stack, looking at: " << op << std::endl;
                exit(-1);
            }
            auto exp = std::move(expression_stack.top()); expression_stack.pop();
            expression_stack.push(std::make_unique<grammar::unary_expression>(std::move(exp), op)));
            */

        } else {
            assert(expression_stack.size() >= 2);

            auto rght = std::move(expression_stack.top()); expression_stack.pop();
            auto lft = std::move(expression_stack.top()); expression_stack.pop();
            expression_stack.push(std::make_unique<grammar::binary_expression>(std::move(lft), op, std::move(rght)));
        }
    };

    while(true) {
        assert(this->peek().type != lexing::token_type::END_OF_FILE);

        if(lexing::utils::op::is_end_of_expression(this->peek()) ||
           (cnt_lparen == 0 && this->peek().type == lexing::token_type::R_PAREN)) {
            break;
        }

        auto current_token = this->advance();

        if(lexing::utils::op::get_precedence(current_token) != -1) {
            if(can_be_unary && lexing::utils::op::can_be_unary(current_token)) {
                current_token = lexing::utils::op::transform_to_unary(current_token);
            }

            while(!operation_stack.empty() && lexing::utils::op::should_pop_top(operation_stack.top(), current_token)) {
                combine_top();
            }

            operation_stack.push(current_token);
            can_be_unary = true;
            continue;

        } else if(current_token.type == lexing::token_type::L_PAREN) {
            operation_stack.push(current_token);
            can_be_unary = true;
            cnt_lparen ++;
            continue;

        } else if(current_token.type == lexing::token_type::R_PAREN) {
            while(!operation_stack.empty() && operation_stack.top().type != lexing::token_type::L_PAREN) {
                combine_top();
            }

            assert(!operation_stack.empty());

            operation_stack.pop();
            cnt_lparen --;

            can_be_unary = false;
            continue;

        } else if(current_token.type == lexing::token_type::IDENTIFIER) {
            if(this->match(lexing::token_type::L_PAREN)) {
                // This is a function call

                // We need to roll back a lil bit
                // Once for the L_PAREN
                // Once for the function name
                this->pit -= 2;
                expression_stack.push(this->parse_function_call());
            } else {
                expression_stack.push(std::make_unique<grammar::identifier_literal>(current_token.value));
            }
            can_be_unary = false;
            continue;

        } else if(current_token.type == lexing::token_type::NUMBER) {
            expression_stack.push(std::make_unique<grammar::number_literal>(current_token.value));
            can_be_unary = false;
            continue;

        } else {
            assert(false);
        }
    }

    while(!operation_stack.empty()) {
        combine_top();
    }

    assert(cnt_lparen == 0);
    assert(expression_stack.size() == 1);

    return std::move(expression_stack.top());
}

std::unique_ptr<grammar::expression_statement> parser::parse_expression_statement() {
    auto expr = this->parse_expression();

    assert(this->match(lexing::token_type::SEMICOLON));

    return std::make_unique<grammar::expression_statement>(std::move(expr));
}

std::unique_ptr<grammar::let_statement> parser::parse_let_statement() {
    assert(this->match(lexing::token_type::LET));

    const auto name_token = this->advance();
    assert(name_token.type == lexing::token_type::IDENTIFIER);

    std::unique_ptr<grammar::type_call> type;

    if(this->match(lexing::token_type::COLON)) {
        type = std::move(this->parse_type_call());
    } else {
        type = nullptr;
    }

    assert(this->match(lexing::token_type::EQUAL));
    auto expr = this->parse_expression();

    assert(this->match(lexing::token_type::SEMICOLON));
    return std::make_unique<grammar::let_statement>(name_token.value, type, std::move(expr));
}

std::unique_ptr<grammar::return_statement> parser::parse_return_statement() {
    assert(this->match(lexing::token_type::RETURN));

    auto expr = this->parse_expression();

    assert(this->match(lexing::token_type::SEMICOLON));
    return std::make_unique<grammar::return_statement>(std::move(expr));
}

std::unique_ptr<grammar::if_statement> parser::parse_if_statement() {
    assert(this->match(lexing::token_type::IF));

    auto cond = this->parse_expression();
	auto body = this->parse_list_statement();

    return std::make_unique<grammar::if_statement>(std::move(cond), std::move(body));
}

std::unique_ptr<grammar::list_statement> parser::parse_list_statement() {
    assert(this->match(lexing::token_type::L_BRACE));

    std::vector<std::unique_ptr<grammar::statement> > body;

    while(this->peek().type != lexing::token_type::R_BRACE) {
        if(this->peek().type == lexing::token_type::LET) {
            body.push_back(this->parse_let_statement());
        } else if(this->peek().type == lexing::token_type::RETURN) {
            body.push_back(this->parse_return_statement());
        } else if(this->peek().type == lexing::token_type::IF) {
            body.push_back(this->parse_if_statement());
        } else {
            body.push_back(this->parse_expression_statement());
        }
	}

    assert(this->match(lexing::token_type::R_BRACE));
    assert(this->match(lexing::token_type::SEMICOLON));
    return std::make_unique<grammar::list_statement>(body);
}

std::unique_ptr<grammar::function_declaration_parameter> parser::parse_function_declaration_parameter() {
    const auto name_token = this->advance();
    assert(name_token.type == lexing::token_type::IDENTIFIER);

    assert(this->match(lexing::token_type::COLON));

    auto type = std::move(this->parse_type_call());

    return std::make_unique<grammar::function_declaration_parameter>(name_token.value, type);
}

std::unique_ptr<grammar::function_declaration> parser::parse_function_declaration() {
    assert(this->match(lexing::token_type::LET));

    const auto name_token = this->advance();
    assert(name_token.type == lexing::token_type::IDENTIFIER);

    assert(this->match(lexing::token_type::EQUAL));
    assert(this->match(lexing::token_type::FUNCTION));

    assert(this->match(lexing::token_type::L_PAREN));

    std::vector<std::unique_ptr<grammar::function_declaration_parameter> > params;
    while(!this->match(lexing::token_type::R_PAREN)) {
        params.push_back(std::move(this->parse_function_declaration_parameter()));

        if(!this->match(lexing::token_type::COMMA)) {
            assert(this->match(lexing::token_type::R_PAREN));
            break;
        }
    }

    const auto type_token = this->advance();
    assert(type_token.type == lexing::token_type::IDENTIFIER);

	// Extern functions have no body
	if(this->match(lexing::token_type::SEMICOLON)) {
		return std::make_unique<grammar::function_declaration>(name_token.value, type_token.value, params);
	}

    auto body = this->parse_list_statement();

    return std::make_unique<grammar::function_declaration>(name_token.value, type_token.value, std::move(body), params);
}

std::unique_ptr<grammar::global_declaration> parser::parse_definition() {
    // Currently doesn't support global constants
    assert(this->match(lexing::token_type::LET));
    assert(this->match(lexing::token_type::IDENTIFIER));
    assert(this->match(lexing::token_type::EQUAL));
    this->pit -= 3;

    return this->parse_function_declaration();
}

std::unique_ptr<grammar::program> parser::parse_program() {
    std::vector<std::unique_ptr<grammar::function_declaration> > function_declarations;

    while(this->peek().type != lexing::token_type::END_OF_FILE) {
        function_declarations.push_back(std::move(this->parse_function_declaration()));
    }

    return std::make_unique<grammar::program>(function_declarations);
}

void context::declare_variable(const std::string &comp, const grammar::memory_cell* definition) {
	this->variables[comp] = definition;
}

std::optional<const grammar::memory_cell*> context::get_variable_definition(const std::string &comp) const {
    const auto res = this->variables.find(comp);

    if(res == this->variables.end()) {
        return std::nullopt;
    } else {
        return std::make_optional(res->second);
    }
}

void context::declare_function(const std::string &comp, const grammar::function_declaration* definition) {
	this->functions[comp] = definition;
}

std::optional<const grammar::function_declaration*> context::get_function_definition(const std::string &comp) const {
    const auto res = this->functions.find(comp);

    if(res == this->functions.end()) {
        return std::nullopt;
    } else {
        return std::make_optional(res->second);
    }
}

};
