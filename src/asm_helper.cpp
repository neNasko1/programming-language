#include <iostream>
#include <cassert>

#include "asm_helper.h"

namespace asm_helper {

void push_to_stack(std::ostream &out, const size_t stack_pos, const size_t size) {
	if(size == 8) {
		out << "\t push qword " << "[rsp+" << stack_pos << "]\n";
	} else if(size == 1) {
		out << "\t mov al, " << "[rsp+" << stack_pos << "]\n";
		out << "\t sub rsp, " << size << "\n";
		out << "\t mov [rsp], al" << "\n";
	} else {
		assert(false);
	}
}

void push_to_stack_str(std::ostream &out, const std::string &&str, const size_t size) {
	out << "\t mov rcx, " << str << "\n";
	if(size == 8) {
		out << "\t push qword " << "[rcx]\n";
	} else if(size == 1) {
		out << "\t mov al, " << "[rcx]\n";
		out << "\t sub rsp, " << size << "\n";
		out << "\t mov [rsp], al" << "\n";
	} else {
		assert(false);
	}
}

void mem_move(std::ostream &out, const std::string &&from, const std::string &&to, const size_t size) {
	if(size == 8) {
		out << "\t mov rax, [" << from << "]\n";
		out << "\t mov [" << to << "], rax\n";
	} else {
		assert(false);
	}
}

};
