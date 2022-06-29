#include <iostream>
#include <string_view>

#include "grammar.h"

namespace grammar {

number_literal::number_literal(const std::string_view value) : value(value) {}

void number_literal::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "number(" << this->value << ")" << std::endl;
}


identifier_literal::identifier_literal(const std::string_view value) : value(value) {}

void identifier_literal::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "identifier(" << this->value << ")" << std::endl;
}


binary_expression::binary_expression(std::unique_ptr<expression> lft, const lexing::token &op, std::unique_ptr<expression> rght)
    : lft(std::move(lft)), op(op), rght(std::move(rght)) {}

void binary_expression::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "binary_expression(" << std::endl;
    this->lft->print(out, ident + 1);
    out << tabulation << "\t" << this->op.value << std::endl;
    this->rght->print(out, ident + 1);
    out << tabulation << ")" << std::endl;
}


expression_statement::expression_statement(std::unique_ptr<expression> expr) : expr(std::move(expr)) {}

void expression_statement::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "expression_statement(" << std::endl;
    this->expr->print(out, ident + 1);
    out << tabulation << ")" << std::endl;
}


list_statement::list_statement(std::vector<std::unique_ptr<statement> > &body) : body(std::move(body)) {}

void list_statement::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "{" << std::endl;
    for(const auto &it : this->body) {
        it->print(out, ident + 1);
    }
    out << tabulation << "}" << std::endl;
}


let_statement::let_statement(const std::string_view name, std::unique_ptr<expression> init_value) : name(name), type(""), init_value(std::move(init_value)) {}

let_statement::let_statement(const std::string_view name, const std::string_view type, std::unique_ptr<expression> init_value) : name(name), type(type), init_value(std::move(init_value)) {}

void let_statement::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    if(this->type.size() == 0) {
        out << tabulation << "let " << this->name << ": " << "auto" << std::endl;
    } else {
        out << tabulation << "let " << this->name << ": " << this->type << std::endl;
    }
    this->init_value->print(out, ident + 1);
}


return_statement::return_statement(std::unique_ptr<expression> value) : value(std::move(value)) {}

void return_statement::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "return " << std::endl;
    this->value->print(out, ident + 1);
}


function_declaration::function_declaration(const std::string_view name, const std::string_view type, std::unique_ptr<statement> body) 
    : name(name), type(type), body(std::move(body)) {}

void function_declaration::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "function " << this->name << " : " << this->type << std::endl;
    this->body->print(out, ident);
}

void program::print(std::ostream &out, const size_t ident) const {
    std::string tabulation = std::string(ident, '\t');

    out << tabulation << "program(" << std::endl;
    for(const auto &it : this->definitions) {
        it->print(out, ident + 1);
    }
    out << tabulation << ")" << std::endl;
}

};