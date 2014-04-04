#ifndef _PSC_PASCAL_STMNT_PARSER_H_
#define _PSC_PASCAL_STMNT_PARSER_H_

#include "im/icode.h"
#include "utils/fwd.h"
#include "pascal/nt_parser.h"
#include <memory>

FWD_DECL_NS_NS_CL(psc, fe, Token)
FWD_DECL_NS_NS_CL(psc, fe, TokenType)
FWD_DECL_NS_NS_CL(psc, im, SymbolTableStack)
FWD_DECL_NS_NS_CL(psc, msg, MessageProducer)

namespace psc {
namespace pascal {

class ErrorCode;
class Scanner;

extern const SynchronizationSet STMNT_FOLLOW_SET;

class StatementParser : public NonTerminalParser
{
public:
	StatementParser(Scanner &s, im::SymbolTableStack &ss, const msg::MessageProducer &mp)
		: NonTerminalParser(s, ss, mp) {}

	std::unique_ptr<im::ICodeNode> parse(const fe::Token &current);

	void parse_list(
		const fe::Token &current,
		im::ICodeNode &parent,
		const fe::TokenType *terminator,
		const ErrorCode &error);
private:
	void set_line(im::ICodeNode &node, int line);
};

}
}

#endif // _PSC_PASCAL_STMNT_PARSER_H_
