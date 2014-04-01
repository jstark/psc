#ifndef _PSC_PASCAL_NT_PARSER_H_
#define _PSC_PASCAL_NT_PARSER_H_

#include "utils/fwd.h"
#include <unordered_set>

FWD_DECL_NS_NS_CL(psc, msg, MessageProducer)
FWD_DECL_NS_NS_CL(psc, fe, Token)
FWD_DECL_NS_NS_CL(psc, fe, TokenType)
FWD_DECL_NS_NS_CL(psc, im, SymbolTableStack)

namespace psc {
namespace pascal {

class Scanner;
using SynchronizationSet = std::unordered_set<const fe::TokenType *>;

struct NonTerminalParser
{
	explicit NonTerminalParser(Scanner &s, im::SymbolTableStack &ss, const msg::MessageProducer &mp)
	: _scanner(s), _symtabstack(ss), _mp(mp) {}

	Scanner &_scanner;
	im::SymbolTableStack &_symtabstack;
	const msg::MessageProducer &_mp;

	fe::Token synchronize(const SynchronizationSet &s);
};

}
}

#endif // _PSC_PASCAL_NT_PARSER_H_