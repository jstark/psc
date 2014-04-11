#ifndef _PSC_PSC_BE_EXPR_INTERPRETER_H_INCLUDED
#define _PSC_PSC_BE_EXPR_INTERPRETER_H_INCLUDED

#include "utils/fwd.h"
#include <string>
#include <boost/variant.hpp>
#include <memory>

FWD_DECL_NS_NS_CL(psc, msg, MessageProducer)
FWD_DECL_NS_NS_CL(psc, im, ICodeNode)

namespace psc {
namespace be {

using ExprVal = boost::variant<bool, int, double, std::string, void *>;

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
