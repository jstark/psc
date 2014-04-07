#include "pascal/repeat_parser.h"
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

std::unique_ptr<ICodeNode> RepeatParser::parse(const Token &current)
{
	auto tok = current;
	tok = _scanner.next(); // consume the REPEAT

	// create the LOOP and TEST nodes.
	auto loop = ICodeFactory::create_node(ICodeNodeType::LOOP);
	auto test = ICodeFactory::create_node(ICodeNodeType::TEST);

	// parse the statement list terminated by the UNTIL token.
	// the LOOP node is the parent of the statement subtrees.
	StatementParser stmnt_parser{ _scanner, _symtabstack, _mp };
	stmnt_parser.parse_list(tok, *loop, &UNTIL, MISSING_UNTIL);
	tok = _scanner.current();

	// parse the expression.
	// the TEST node adopts the expression subtree as its only child.
	// the LOOP node adopts the TEST node.
	ExprParser expr_parser{ _scanner, _symtabstack, _mp };
	test->add_child(expr_parser.parse(tok));
	loop->add_child(std::move(test));

	return loop;
}
