#pragma once

#include <iostream>
#include <vector>
#include <string>

namespace lexing {

enum token_type {
    //Keywords
    ELSE, FUNCTION, FOR, IF, RETURN, LET, WHILE,
    //Operators
    PLUS, MINUS, STAR, SLASH, MODULO, OR, AND, XOR, NOT,
    PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL, MODULO_EQUAL, OR_EQUAL, AND_EQUAL, XOR_EQUAL, EQUAL,
    //Boolean operators
    BANG, BANG_EQUAL, EQUAL_EQUAL, LESS, LESS_EQUAL, GREATER, GREATER_EQUAL, OROR, ANDAND, XORXOR,
    //Unary
    UNARY_PLUS, UNARY_MINUS, UNARY_REFERENCE, UNARY_DEREFERENCE,
    //Separators
    COMMA, SEMICOLON, DOT, COLON, QUESTION_MARK,
    //Brackets
    L_BRACE, R_BRACE, L_PAREN, R_PAREN, L_SQUARE_BRACKET, R_SQUARE_BRACKET,
    //Literals
    CHARACTER, NUMBER, BOOLEAN, STRING, IDENTIFIER,
    NEW_LINE, END_OF_FILE, INVALID,
    TOKEN_TYPE_SIZE
};

// Only used for debugging purposes
const std::string reverse_token_type_names[token_type::TOKEN_TYPE_SIZE] = {
    "else", "function", "for", "if", "return", "let", "while",
    "+", "-", "*", "/", "%", "|", "&", "^", "~",
    "+=", "-=", "*=", "/=", "%=", "|=", "&=", "^=", "=",
    "!", "!=", "==", "<", "<=", ">", ">=", "||", "&&", "^^",
    "unary +", "unary -", "unary &", "unary *",
    ",", ";", ".", ":", "?",
    "{", "}", "(", ")", "[", "]",
    "character", "number", "boolean", "string", "name",
    "NEW_LINE", "EOF", "INVALID"
};


struct token {
	std::string value;
	token_type type;

	token(const std::string &value, const token_type type);
	~token();
};

std::ostream& operator <<(std::ostream &out, const token &tkn);

struct lexer {
	const std::string code;
	size_t pic; // Position in code(index of the next character which should be read)
	std::vector<token> tokens;

	lexer(const std::string &code);
	~lexer();

	char peek() const;
	char advance();
    bool match(const char c);
	bool is_at_end() const;
	void lex();
    token recognise_special();
};

namespace utils {

namespace character {

bool is_alphabet(const char c);
bool is_digit(const char c);
bool is_alphabet_or_digit(const char c);
bool is_name_start(const char c);
bool is_name_continuation(const char c);
bool is_whitespace(const char c);
bool is_special(const char c);

};

namespace word {

token_type get_word_type(const std::string &word);

};

namespace op {

bool can_be_unary(const lexing::token &tkn);
bool is_unary(const lexing::token &tkn);
int32_t get_precedence(const lexing::token &tkn);
bool is_left_associative(const lexing::token &tkn);
lexing::token transform_to_unary(const lexing::token &tkn);
bool is_end_of_expression(const lexing::token &tkn);
bool should_pop_top(const lexing::token &top, const lexing::token &nw);

};

};

};
