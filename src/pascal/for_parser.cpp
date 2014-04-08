#include "pascal/for_parser.h"
#include "pascal/stmnt_parser.h"
#include "pascal/expr_parser.h"
#include "pascal/assignment_parser.h"
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

std::unique_ptr<ICodeNode> ForParser::parse(const Token &current)
{
	auto tok = current;
	tok = _scanner.next(); // consume the FOR token
	auto target = tok;

	// create the COMPOUND, LOOP and TEST nodes
	auto compound = ICodeFactory::create_node(ICodeNodeType::COMPOUND);
	auto loop = ICodeFactory::create_node(ICodeNodeType::LOOP);
	auto test = ICodeFactory::create_node(ICodeNodeType::TEST);

	// parse the embedded initial assignment
	AssignmentParser assign{ _scanner, _symtabstack, _mp };
	auto init_assign_node = assign.parse(tok);

	// set the current line number 
	init_assign_node->set_attribute(ICodeKey::LINE, tok.line_number());

	// the COMPOUND node adopts the initial ASSIGN and the LOOP nodes
	// as its first and second children.
	compound->add_child(std::move(init_assign_node));
	compound->add_child(std::move(loop));

	// synchronize at the TO or DOWNTO
	tok = synchronize(to_downto_set());
	auto direction = tok.type();
	if (direction == &TO || direction == &DOWNTO)
	{
		tok = _scanner.next();
	}
	else
	{
		direction = &TO;
		ErrorHandler::flag(tok, &MISSING_TO_DOWN_TO, _mp);
	}

	// create a relational operator node: GT for TO, LT for DOWNTO
	auto rel_op= ICodeFactory::create_node(direction == &TO ? ICodeNodeType::GT : ICodeNodeType::LT);

	// copy the control VARIABLE node. The relational operator node
	// adopts the copied VARIABLE node, as its first child.
	auto children = init_assign_node->children();
	auto control_var = children.at(0);
	rel_op->add_child(control_var->copy());

	// parse the termination expression. The relational operator node
	// adopts the expression as its second child.
	ExprParser expr_parser{ _scanner, _symtabstack, _mp };
	rel_op->add_child(expr_parser.parse(tok));

	// the TEST node adopts the relational operator node as its only child.
	// the LOOP node adopts the TEST node as its first child.
	test->add_child(std::move(rel_op));
	test->add_child(std::move(test));

	// synchronize at the DO
	tok = synchronize(do_set());
	if (tok.type() == &DO)
	{
		tok = _scanner.next();
	}
	else
	{
		ErrorHandler::flag(tok, &MISSING_DO, _mp);
	}

	// parse the nested statement. The LOOP node adopts the statement
	// node as its second child.
	StatementParser stmnt_parser{ _scanner, _symtabstack, _mp };
	loop->add_child(stmnt_parser.parse(tok));

	// create an assignment with a copy of the control variable
	// to advance the value of the variable
	auto next_assign = ICodeFactory::create_node(ICodeNodeType::ASSIGN);
	next_assign->add_child(control_var->copy());

	// create an arithmetic operator node
	// ADD for TO, SUBTRACT for DOWNTO
	auto arith_node = ICodeFactory::create_node(direction == &TO ? ICodeNodeType::ADD : ICodeNodeType::SUBTRACT);

	// the operator node adopts a copy of the loop variable as its 
	// first child and the value 1 as its second child.
	arith_node->add_child(std::move(control_var->copy()));
	auto one_node = ICodeFactory::create_node(ICodeNodeType::INTEGER_CONSTANT);
	one_node->set_attribute(ICodeKey::VALUE, 1);
	arith_node->add_child(std::move(one_node));

	// the next ASSIGN node adopts the arithmetic operator node as its 
	// second child. The LOOP node adopts the next ASSIGN node as its third child.
	next_assign->add_child(std::move(arith_node));
	loop->add_child(std::move(next_assign));

	// set the current line number attribute
	next_assign->set_attribute(ICodeKey::LINE, target.line_number());

	return compound;
}

SynchronizationSet ForParser::to_downto_set()
{
	auto to_downto = StatementParser::follow_set();
	to_downto.insert(&TO);
	to_downto.insert(&DOWNTO);
	auto tmp = ExprParser::start_set();
	to_downto.insert(tmp.begin(), tmp.end());
	return to_downto;
}

SynchronizationSet ForParser::do_set()
{
	auto do_set = StatementParser::follow_set();
	auto tmp = StatementParser::start_set();
	do_set.insert(tmp.begin(), tmp.end());
	do_set.insert(&DO);
	return do_set;
}

