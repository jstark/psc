#ifndef _PSC_PSC_BE_CASE_INTERPRETER_H_INCLUDED
#define _PSC_PSC_BE_CASE_INTERPRETER_H_INCLUDED

#include "utils/fwd.h"
#include <memory>
#include <boost/variant.hpp>

FWD_DECL_NS_NS_CL(psc, msg, MessageProducer)
FWD_DECL_NS_NS_CL(psc, im, ICodeNode)

namespace psc {
namespace be {

using CaseConstant = boost::variant<int, std::string>;

class CaseInterpreter final
{
public:
    explicit CaseInterpreter(const msg::MessageProducer &mp)
        : _mp(mp) {}

    void* execute(const im::ICodeNode &node, int *exec_count);

private:
    const msg::MessageProducer &_mp;
};

}
}

#endif // _PSC_PSC_BE_CASE_INTERPRETER_H_INCLUDED
