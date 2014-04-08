#ifndef _PSC_PASCAL_FOR_PARSER_H_INCLUDED_
#define _PSC_PASCAL_FOR_PARSER_H_INCLUDED_

#include "im/icode.h"
#include "utils/fwd.h"
#include "pascal/nt_parser.h"
#include <memory>

FWD_DECL_NS_NS_CL(psc, fe, Token)
FWD_DECL_NS_NS_CL(psc, im, SymbolTableStack)
FWD_DECL_NS_NS_CL(psc, msg, MessageProducer)

namespace psc {
namespace pascal {

class Scanner;

class ForParser : public NonTerminalParser
{
public:
	ForParser(Scanner &s, im::SymbolTableStack &ss, const msg::MessageProducer &mp)
		: NonTerminalParser(s, ss, mp) {}

	std::unique_ptr<im::ICodeNode> parse(const fe::Token &current);
	
	static SynchronizationSet to_downto_set();
	static SynchronizationSet do_set();
};

}
}

#endif // _PSC_PASCAL_FOR_PARSER_H_INCLUDED_