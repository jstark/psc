#include "pascal/stmnt_parser.h"
#include "pascal/compound_parser.h"
#include "pascal/assignment_parser.h"
#include "pascal/while_parser.h"
#include "pascal/repeat_parser.h"
#include "pascal/for_parser.h"
#include "pascal/if_parser.h"
#include "pascal/case_parser.h"
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
	else if (type == &IDENTIFIER)
	{
		AssignmentParser assignment_parser{ _scanner, _symtabstack, _mp };
		node = assignment_parser.parse(current);
	}
	else if (type == &REPEAT)
	{
		RepeatParser repeat_parser{ _scanner, _symtabstack, _mp };
		node = repeat_parser.parse(current);
	}
	else if (type == &WHILE)
	{
		WhileParser while_parser{ _scanner, _symtabstack, _mp };
		node = while_parser.parse(current);
	}
	else if (type == &FOR)
	{
		ForParser for_parser{ _scanner, _symtabstack, _mp };
		node = for_parser.parse(current);
	}
	else if (type == &IF)
	{
		IfParser if_parser{ _scanner, _symtabstack, _mp };
		node = if_parser.parse(current);
	}
	else if (type == &CASE)
	{
		CaseParser case_parser{ _scanner, _symtabstack, _mp };
		node = case_parser.parse(current);
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
	auto stmnt_start_set = StatementParser::start_set();
	SyncSet sync(stmnt_start_set.begin(), stmnt_start_set.end());
	sync.insert(terminator);

	auto tok = current;
	while (!tok.is_eof() && tok.type() != terminator)
	{
		// parse a statement. the parent adopts the statement node.
		parent.add_child(parse(tok));
		tok = _scanner.current();

		// look for the semicolon between the statements.
		if (tok.type() == &SEMICOLON)
		{
			tok = _scanner.next(); // consume it.
		}
		else if (stmnt_start_set.count(tok.type()))
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

SynchronizationSet StatementParser::follow_set()
{
	// synchronization set for following a statement.
	return  { &SEMICOLON, &END, &ELSE, &UNTIL, &DOT };
}

SynchronizationSet StatementParser::start_set()
{
	// synchronization set for starting a statement.
	return { &BEGIN, &CASE, &FOR, &IF, &REPEAT, &WHILE, &IDENTIFIER, &SEMICOLON };
}
