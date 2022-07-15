#include <iostream>
#include <vector>
#include <cassert>

#include "lexing.h"

namespace lexing {

token::token(const std::string_view value, const token_type type) : value(value), type(type) {}
token::~token() {}

std::ostream& operator <<(std::ostream &out, const token &tkn) {
    if(tkn.type == token_type::NEW_LINE) {
        return out << "\t \\n" << " " << reverse_token_type_names[tkn.type];
    } else {
        return out << tkn.value << " " << reverse_token_type_names[tkn.type];
    }
}


lexer::lexer(const std::string &code) : code(std::move(code)), pic(0), tokens() {}
lexer::~lexer() {}

char lexer::peek() const {
	return this->code[this->pic];
}

char lexer::advance() {
	return this->code[this->pic ++];
}

bool lexer::match(const char c) {
    if(this->peek() == c) {
        return this->advance(), true;
    } else {
        return false;
    }
}

bool lexer::is_at_end() const {
	return this->pic >= this->code.size();
}

token lexer::recognise_special() {
    assert(!this->is_at_end());

    #define RETURN_TOKEN(length, type) return token(std::string_view(this->code).substr(this->pic - length, 1), type);

    if(this->match('+')) {
        RETURN_TOKEN(1, token_type::PLUS);
    } else if(this->match('-')) {
        RETURN_TOKEN(1, token_type::MINUS);
    } else if(this->match('*')) {
        RETURN_TOKEN(1, token_type::STAR);
    } else if(this->match('/')) {
        RETURN_TOKEN(1, token_type::SLASH);
    } else if(this->match('%')) {
        RETURN_TOKEN(1, token_type::MODULO);
    } else if(this->match('=')) {
        RETURN_TOKEN(1, token_type::EQUAL);
    } else if(this->match('(')) {
        RETURN_TOKEN(1, token_type::L_PAREN);
    } else if(this->match(')')) {
        RETURN_TOKEN(1, token_type::R_PAREN);
    } else if(this->match('{')) {
        RETURN_TOKEN(1, token_type::L_BRACE);
    } else if(this->match('}')) {
        RETURN_TOKEN(1, token_type::R_BRACE);
    } else if(this->match('[')) {
        RETURN_TOKEN(1, token_type::L_SQUARE_BRACKET);
    } else if(this->match(']')) {
        RETURN_TOKEN(1, token_type::R_SQUARE_BRACKET);
    } else if(this->match(';')) {
        RETURN_TOKEN(1, token_type::SEMICOLON);
    } else if(this->match(',')) {
        RETURN_TOKEN(1, token_type::COMMA);
    } else if(this->match(':')) {
        RETURN_TOKEN(1, token_type::COLON);
    }

    #undef RETURN_TOKEN

    assert(false);
}

void lexer::lex() {
	while(!this->is_at_end()) {
		if(utils::character::is_alphabet(this->peek())) {
			const size_t start = this->pic;
			while(!this->is_at_end() && utils::character::is_alphabet_or_digit(this->peek())) {
				this->advance();
			}
			const size_t end   = this->pic - 1;

            const std::string_view value = std::string_view(this->code).substr(start, end - start + 1);
            const token_type type = utils::word::get_word_type(value);
			this->tokens.push_back(token(value, type));

		} else if(utils::character::is_whitespace(this->peek())) {
			if(this->advance() == '\n') {
                if(!this->tokens.empty() && this->tokens.back().type != token_type::SEMICOLON && this->tokens.back().type != token_type::L_BRACE) {
				    this->tokens.push_back(token(";", token_type::SEMICOLON));
                }
			}

        } else if(utils::character::is_special(this->peek())) {
            this->tokens.push_back(this->recognise_special());

        } else if(utils::character::is_digit(this->peek())) {
			const size_t start = this->pic;
			while(!this->is_at_end() && utils::character::is_digit(this->peek())) {
				this->advance();
			}
			const size_t end   = this->pic - 1;

            const std::string_view value = std::string_view(this->code).substr(start, end - start + 1);
            const token_type type = token_type::NUMBER;
			this->tokens.push_back(token(value, type));

		} else {
			std::cerr << "Unmatched character in lexing " << this->advance() << std::endl;
            continue;
		}
	}

    tokens.push_back(token("", token_type::END_OF_FILE));
}

namespace utils {

namespace character {

bool is_alphabet(const char c) {
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool is_digit(const char c) {
	return '0' <= c && c <= '9';
}

bool is_alphabet_or_digit(const char c) {
	return is_alphabet(c) || is_digit(c);
}

bool is_whitespace(const char c) {
	return c == ' ' || c == '\n' || c == '\t';
}

bool is_special(const char c) {
    return
        c == '+' ||
        c == '-' ||
        c == '*' ||
        c == '/' ||
        c == '%' ||
        c == '=' ||
        c == '(' ||
        c == ')' ||
        c == '{' ||
        c == '}' ||
        c == '[' ||
        c == ']' ||
        c == ';' ||
        c == ',' ||
        c == ':';
}

};

namespace word {

// TODO: SLOW -> use a trie
token_type get_word_type(const std::string_view word) {
    if(word == "else") {
        return token_type::ELSE;
    } else if(word == "func") {
        return token_type::FUNCTION;
    } else if(word == "for") {
        return token_type::FOR;
    } else if(word == "if") {
        return token_type::IF;
    } else if(word == "return") {
        return token_type::RETURN;
    } else if(word == "let") {
        return token_type::LET;
    } else if(word == "while") {
        return token_type::WHILE;
    }

    return token_type::IDENTIFIER;
}

};

namespace op {

// Taken from https://github.com/neNasko1/xcppcompiler/blob/main/src/Lexer.h
const int32_t precedence[lexing::token_type::TOKEN_TYPE_SIZE] = {
    -1, -1, -1, -1, -1, -1, -1,
    12, 12, 10, 10, 10, 26, 22, 24, 7,
    35, 35, 35, 35, 35, 35, 35, 35, 35,
    7, 20, 20, 18, 18, 18, 18, 32, 28, 30,
    7, 7, 7, 7, // ??? Not sure
    -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1,
    -1, -1, -1
};

bool can_be_unary(const lexing::token &tkn) {
    switch(tkn.type) {
        case lexing::token_type::PLUS: return true;
        case lexing::token_type::MINUS: return true;
        case lexing::token_type::NOT: return true;
        default: return false;
    }
    assert(false);
}

bool is_unary(const lexing::token &tkn) {
    return precedence[tkn.type] == 7;
}

// Consult https://github.com/neNasko1/xcppcompiler/blob/main/src/Lexer.h
// -> or consult c++ standart implementation
// Smaller = more priority
int32_t get_precedence(const lexing::token &tkn) {
    return precedence[tkn.type];
}

bool is_left_associative(const lexing::token &tkn) {
    switch(tkn.type) {
        case lexing::token_type::EQUAL: return false;
        case lexing::token_type::UNARY_PLUS: return false;
        case lexing::token_type::UNARY_MINUS: return false;
        case lexing::token_type::NOT: return false;
        default: return true;
    }
    return true;
}

lexing::token transform_to_unary(const lexing::token &tkn) {
    switch(tkn.type) {
        case lexing::token_type::PLUS:
            return lexing::token(tkn.value, lexing::token_type::UNARY_PLUS);
        case lexing::token_type::MINUS:
            return lexing::token(tkn.value, lexing::token_type::UNARY_MINUS);
        case lexing::token_type::NOT:
            return tkn;
        case lexing::token_type::UNARY_REFERENCE:
            return tkn;
        case lexing::token_type::UNARY_DEREFERENCE:
            return tkn;
        default: return tkn;
    }
    return tkn;
}

bool is_end_of_expression(const lexing::token &tkn) {
    switch(tkn.type) {
        case lexing::token_type::SEMICOLON: return true;
        case lexing::token_type::COMMA:     return true;
        case lexing::token_type::L_BRACE:   return true;
        case lexing::token_type::R_BRACE:   return true;
        default: return false;
    }
    return false;
}

bool should_pop_top(const lexing::token &top, const lexing::token &nw) {
    return top.type != lexing::token_type::L_PAREN && (
            get_precedence(top) < get_precedence(nw) || (get_precedence(top) == get_precedence(nw) && is_left_associative(nw))
        );
}

};

};

};
