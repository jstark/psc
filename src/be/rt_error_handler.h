#ifndef _PSC_BE_RUNTIME_ERROR_HANDLER_H_INCLUDED
#define _PSC_BE_RUNTIME_ERROR_HANDLER_H_INCLUDED

#include "utils/fwd.h"

FWD_DECL_NS_NS_CL(psc, im, ICodeNode)
FWD_DECL_NS_NS_CL(psc, msg, MessageProducer)

namespace psc {
namespace be {

class RuntimeErrorCode;

class RuntimeErrorHandler final
{
public:
	static void flag(const im::ICodeNode &node, const RuntimeErrorCode &error, const msg::MessageProducer &mp);
	static int error_count() { return _errors; }
private:
	RuntimeErrorHandler() = delete;
	static const int MAX_ERRORS = 5;
	static int _errors;
};

}
}

#endif // _PSC_BE_RUNTIME_ERROR_HANDLER_H_INCLUDED