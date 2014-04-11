#ifndef _PSC_PSC_BE_EXPR_INTERPRETER_H_INCLUDED
#define _PSC_PSC_BE_EXPR_INTERPRETER_H_INCLUDED

#include "utils/fwd.h"
#include <string>
#include <sstream>
#include <boost/variant.hpp>
#include <memory>

FWD_DECL_NS_NS_CL(psc, msg, MessageProducer)
FWD_DECL_NS_NS_CL(psc, im, ICodeNode)

namespace psc {
namespace be {

using ExprVal = boost::variant<bool, int, double, std::string, void *>;

struct ExprValToString : public boost::static_visitor<std::string>
{
	std::string operator()(bool b) const
	{
		return b ? "True" : "False";
	}

	std::string operator()(int i) const
	{
		return std::to_string(i);
	}

	std::string operator()(double d) const
	{
		std::ostringstream out;
		out << d;
		return out.str();
	}

	std::string operator()(const std::string& s) const
	{
		return s;
	}

	std::string operator()(const void *p) const
	{
		return "object";
	}
};

class ExprInterpreter final
{
public:
    explicit ExprInterpreter(const msg::MessageProducer &mp)
        : _mp(mp) {}

    ExprVal execute(const im::ICodeNode &node, int *exec_count);

private:
    ExprVal execute_binop(const im::ICodeNode &node, int *exec_count);

    const msg::MessageProducer &_mp;
};

}
}

#endif // _PSC_PSC_BE_EXPR_INTERPRETER_H_INCLUDED
