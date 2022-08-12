#include <iostream>

#include "asm_helper.h"

namespace asm_helper {

void push_to_stack(std::ostream &out, const size_t stack_pos, const size_t size) {
	if(size == 8) {
		out << "\t push qword " << "[rsp+" << stack_pos << "]\n";
	} else if(size == 1) {
		out << "\t mov al, " << "[rsp+" << stack_pos << "]\n";
		out << "\t sub rsp, " << size << "\n";
		out << "\t mov [rsp], al" << "\n";
	}
}

};
