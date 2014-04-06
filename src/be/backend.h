#ifndef _PSC_BE_BACKEND_H_INCLUDED_
#define _PSC_BE_BACKEND_H_INCLUDED_

#include "utils/fwd.h"
#include "msg/message.h"
#include <memory>
#include <string>

FWD_DECL_NS_NS_CL(psc, im, ICode);
FWD_DECL_NS_NS_CL(psc, im, SymbolTable);

namespace psc { 
namespace be {

class Backend
{
public:
	virtual ~Backend() {}

	virtual void process(std::unique_ptr<im::ICode> iCode, std::unique_ptr<im::SymbolTable> symtab) = 0;
	static std::unique_ptr<Backend> create(const std::string &type);
};

};
}

#endif // _PSC_BE_BACKEND_H_INCLUDED_