#include "pascal/while_parser.h"
#include "pascal/stmnt_parser.h"
#include "pascal/expr_parser.h"
#include "pascal/scanner.h"
#include "pascal/token_type.h"
#include "pascal/error_handler.h"
#include "pascal/error.h"
#include "im/symbol_table.h"
#include "fe/token.h"

#include <algorithm>
#include <iterator>

using namespace psc::fe;
using namespace psc::im;
using namespace psc::pascal;

namespace
{
	SynchronizationSet make_do_set()
	{
		SynchronizationSet do_only = StatementParser::follow_set();
		do_only.insert(&DO);
		return do_only;
	}

	const SynchronizationSet DO_SET = make_do_set();
}

std::unique_ptr<ICodeNode> WhileParser::parse(const Token &current)
{
	auto tok = current;
	tok = _scanner.next(); // consume the WHILE

	// create LOOP, TEST and NOT nodes
	auto loop = ICodeFactory::create_node(ICodeNodeType::LOOP);
	auto exit = ICodeFactory::create_node(ICodeNodeType::TEST);
	auto notn = ICodeFactory::create_node(ICodeNodeType::NOT);

	ExprParser expr_parser{ _scanner, _symtabstack, _mp };
	notn->add_child(expr_parser.parse(tok));

	// the LOOP node adopts the TEST node as its first child.
	// the TEST node adopts the NOT node as its only child.
	exit->add_child(std::move(notn));
	loop->add_child(std::move(exit));

	// synchronize at the DO
	tok = synchronize(DO_SET);
	if (tok.type() == &DO)
	{
		tok = _scanner.next();
	}
	else
	{
		ErrorHandler::flag(tok, &MISSING_DO, _mp);
	}

	// parse the statement
	// the LOOP node adopts the statement subtree as its second child
	StatementParser stmnt_parser{ _scanner, _symtabstack, _mp };
	loop->add_child(stmnt_parser.parse(tok));

	return loop;
}
