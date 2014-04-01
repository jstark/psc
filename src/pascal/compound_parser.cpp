#include "pascal/compound_parser.h"
#include "pascal/stmnt_parser.h"
#include "pascal/scanner.h"
#include "pascal/token_type.h"
#include "pascal/error.h"
#include "fe/token.h"

using namespace psc::fe;
using namespace psc::im;
using namespace psc::pascal;

std::unique_ptr<ICodeNode> CompoundParser::parse(const Token &current)
{
	// ignore argument
	auto tok = _scanner.next();

	// create a COMPOUND node
	auto compound = ICodeFactory::create_node(ICodeNodeType::COMPOUND);

	// parse the statement list terminated by an END token
	StatementParser stmnt_parser{ _scanner, _symtabstack, _mp };
	stmnt_parser.parse_list(tok, *compound, &END, MISSING_END);

	return compound;
}