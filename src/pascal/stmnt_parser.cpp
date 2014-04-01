#include "pascal/stmnt_parser.h"
#include "pascal/compound_parser.h"
#include "pascal/scanner.h"
#include "pascal/token_type.h"
#include "pascal/error_handler.h"
#include "pascal/error.h"
#include "fe/token.h"

#include <unordered_set>
#include <unordered_map>

using namespace psc::fe;
using namespace psc::im;
using namespace psc::pascal;


using std::unique_ptr;
using std::unordered_set;

namespace
{
	using SyncSet = unordered_set<const psc::fe::TokenType *>;

	// synchronization set for starting a statement.
	const SyncSet STMNT_START_SET = { &BEGIN, &CASE, &FOR, &IF, &REPEAT, &WHILE, &IDENTIFIER, &SEMICOLON };

	// synchronization set for following a statement.
	const SyncSet STMNT_FOLLOW_SET = { &SEMICOLON, &END, &ELSE, &UNTIL, &DOT };
}

unique_ptr<ICodeNode> StatementParser::parse(const Token &current)
{
	unique_ptr<ICodeNode> node;
	auto type = current.type();

	if (type == &BEGIN)
	{
		CompoundParser compound_parser{ _scanner, _symtabstack, _mp };
		node = compound_parser.parse(current);
	}
	else
	{
		node = ICodeFactory::create_node(ICodeNodeType::NO_OP);
	}

	set_line(*node, current.line_number());
	return node;
}

void StatementParser::parse_list(const Token &current, ICodeNode &parent, const psc::fe::TokenType *terminator, const ErrorCode &error)
{
	SyncSet sync(STMNT_START_SET.begin(), STMNT_START_SET.end());
	sync.insert(terminator);

	auto tok = current;
	while (!current.is_eof() && current.type() != terminator)
	{
		// parse a statement. the parent adopts the statement node.
		parent.add_child(parse(current));
		tok = _scanner.current();

		// look for the semicolon between the statements.
		if (tok.type() == &SEMICOLON)
		{
			tok = _scanner.next(); // consume it.
		}
		else if (STMNT_START_SET.count(tok.type()))
		{
			ErrorHandler::flag(tok, &MISSING_SEMICOLON, _mp);
		}

		// synchronize at the start of the next statement
		// or at the terminator
		tok = synchronize(sync);
	}

	// look for the terminator token
	if (tok.type() == terminator)
	{
		_scanner.next(); // consume terminator
	}
	else
	{
		ErrorHandler::flag(tok, &error, _mp);
	}
}

void StatementParser::set_line(ICodeNode &node, int line)
{
	node.set_attribute(ICodeKey::LINE, line);
}
