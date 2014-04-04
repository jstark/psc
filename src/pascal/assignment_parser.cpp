#include "pascal/assignment_parser.h"
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
	SynchronizationSet colon_equals()
	{
		std::unordered_set<const psc::fe::TokenType *> ce = { &COLON_EQUALS };
		std::set_union(
			STMNT_FOLLOW_SET.begin(), STMNT_FOLLOW_SET.end(),
			EXPR_START.begin(), EXPR_START.end(),
			std::inserter(ce, ce.begin())
			);
		return ce;
	}

	const SynchronizationSet COLON_EQUALS_SET = colon_equals();

}

std::unique_ptr<ICodeNode> AssignmentParser::parse(const Token &current)
{
	auto tok = current;
	// create the ASSIGN node
	auto assign = ICodeFactory::create_node(ICodeNodeType::ASSIGN);

	// lookup the target identifier in the symbol table stack.
	// enter the identifier into the table if it's not found.
	auto target_name = tok.lexeme();
	std::transform(target_name.begin(), target_name.end(), target_name.begin(), ::tolower);
	auto target_id_opt = _symtabstack.lookup_local(target_name);
	auto target_id = target_id_opt.get_value_or(nullptr);
	if (target_id == nullptr)
	{
		target_id = _symtabstack.enter_local(target_name);
	}

	target_id->append_line(tok.line_number());

	tok = _scanner.next(); // consume the IDENTIFIER token

	// create the variable node and set its name attribute.
	auto variable = ICodeFactory::create_node(ICodeNodeType::VARIABLE);
	variable->set_attribute(ICodeKey::ID, target_id);

	// the assign node adopts the variable node as its first child
	assign->add_child(std::move(variable));

	// synchronize on the := token.
	tok = synchronize(COLON_EQUALS_SET);

	// look for the := token
	if (tok.type() == &COLON_EQUALS)
	{
		tok = _scanner.next(); // consume the :=
	}
	else
	{
		ErrorHandler::flag(tok, &MISSING_COLON_EQUALS, _mp);
	}

	// parse the expression. The ASSIGN node adopts the expression's node
	// as its second child.
	ExprParser expr_parser{ _scanner, _symtabstack, _mp };
	assign->add_child(expr_parser.parse(tok));
	
	return assign;
}
