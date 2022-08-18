#pragma once

#include <iostream>
#include <string>

namespace asm_helper {

void push_to_stack(std::ostream &out, const size_t stack_pos, const size_t size);
void push_to_stack_str(std::ostream &out, const std::string &&str, const size_t size);

};
