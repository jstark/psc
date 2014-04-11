#ifndef _PSC_BE_INTERPRETER_H_INCLUDED_
#define _PSC_BE_INTERPRETER_H_INCLUDED_

#include "utils/fwd.h"
#include "msg/message.h"
#include "be/backend.h"
#include <memory>
#include <string>

FWD_DECL_NS_NS_CL(psc, im, ICode);
FWD_DECL_NS_NS_CL(psc, im, SymbolTableStack);

namespace psc {
namespace be {

class Interpreter final : public Backend
{
public:
	friend class Backend;
	explicit Interpreter(psc::msg::MessageProducer &mp)
		: Backend(mp) {}
    void process(std::unique_ptr<im::ICode> iCode, std::unique_ptr<im::SymbolTableStack> symtab) override;
private:
	Interpreter();
};

};
}

#endif // _PSC_BE_INTERPRETER_H_INCLUDED_
