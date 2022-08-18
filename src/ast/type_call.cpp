#include <string_view>
#include <memory>
#include <string>
#include <cassert>

#include "../grammar.h"
#include "../typing.h"

namespace grammar {

type_call::type_call(const std::string &name) : name(name), args(), internal_name(name), type_ind(typing::NOT_INFERED_ID) {}

type_call::type_call(const std::string &name, std::vector<std::unique_ptr<type_call> > &args) : name(name), args(std::move(args)), internal_name(name), type_ind(typing::NOT_INFERED_ID) {
    bool first = true;
    for(const auto &it : this->args) {
        if(first) {
            this->internal_name += '<';
            first = false;
        } else {
            this->internal_name += ',';
        }
        this->internal_name += it->internal_name;
    }
    if(!this->args.empty()) {
        this->internal_name += '>';
    }
}

void type_call::print(std::ostream &out, const size_t ident) const {
    const std::string tabulation = std::string(ident, '\t');

    out << tabulation << this->internal_name << std::endl;
}

void type_call::compile(std::ostream &out, parsing::context &ctx) {
    const auto found_type = ctx.type_system.find_type(this->internal_name);
    if(found_type) {
        this->type_ind = found_type.value();
        return;
    }

    for(auto &arg : this->args) {
        arg->compile(out, ctx);
    }

    if(this->name == "&") {
		assert(this->args.size() == 1);
		const auto ref_to = this->args[0]->type_ind;
        const auto now = std::make_shared<typing::reference_type>(this->internal_name, ref_to, &(ctx.type_system));
    	ctx.type_system.add_type(now);
	} else {
        assert(this->args.size() == 0);
        assert("Type doesn't exist" && false);
    }
}

};
