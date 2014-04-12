#include "pascal/case_parser.h"
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
#include <set>
#include <sstream>

using namespace psc::fe;
using namespace psc::im;
using namespace psc::pascal;
using std::unique_ptr;
using std::string;

namespace
{
	SynchronizationSet constant_start_set()
	{
		return{ &IDENTIFIER, &INTEGER, &PLUS, &MINUS, &STRING };
	}

	SynchronizationSet of_set()
	{
		auto tmp = StatementParser::follow_set();
		auto of = constant_start_set();
		of.insert(&OF);
		of.insert(tmp.begin(), tmp.end());
		return of;
	}

	SynchronizationSet comma_set()
	{
		auto comma = constant_start_set();
		comma.insert(&COMMA);
		comma.insert(&COLON);
		auto tmp1 = StatementParser::start_set();
		auto tmp2 = StatementParser::follow_set();
		comma.insert(tmp1.begin(), tmp1.end());
		comma.insert(tmp2.begin(), tmp2.end());
		return comma;
	}
}

unique_ptr<ICodeNode> CaseParser::parse(const Token &current)
{
	auto tok = current;
	tok = _scanner.next(); // consume the CASE token

	// create a SELECT node
	auto select = ICodeFactory::create_node(ICodeNodeType::SELECT);

	// parse the CASE expression.
	// the SELECT node adopts the expression subtree as its first child.
	ExprParser expr_parser{ _scanner, _symtabstack, _mp };
	select->add_child(expr_parser.parse(tok));

	tok = _scanner.current();

	// synchronize the OF token
	static const auto OF_SET = of_set();
	static const auto CONST_START_SET = constant_start_set();
	tok = synchronize(OF_SET);
	if (tok.type() == &OF)
	{
		tok = _scanner.next(); // consume the OF
	}
	else
	{
		ErrorHandler::flag(tok, &MISSING_OF, _mp);
	}

	// set of CASE branch constants.
	ConstantSet cs;

	// loop to parse each CASE branch until the END token
	// or the end of the source file
	while (!tok.is_eof() && tok.type() != &END)
	{
		// the SELECT node adopts the CASE branch subtree
		select->add_child(parse_branch(tok, &cs));
		tok = _scanner.current();
		if (tok.type() == &SEMICOLON)
		{
			tok = _scanner.next(); // consume the ;
		}
		else if (CONST_START_SET.count(tok.type()))
		{
			ErrorHandler::flag(tok, &MISSING_SEMICOLON, _mp);
		}
	}

	// look for the END token.
	if (tok.type() == &END)
	{
		tok = _scanner.next(); // consume the END
	}
	else
	{
		ErrorHandler::flag(tok, &MISSING_END, _mp);
	}
	return select;
}

unique_ptr<ICodeNode> CaseParser::parse_branch(const Token &token, ConstantSet *cs)
{
	auto tok = token;
	// create a SELECT_BRANCH node and a SELECT_CONSTANTS node
	// the SELECT_BRANCH adopts the SELECT_CONSTANTS node as its 
	// first child.
	auto branch = ICodeFactory::create_node(ICodeNodeType::SELECT_BRANCH);
	auto constants = ICodeFactory::create_node(ICodeNodeType::SELECT_CONSTANTS);

	// parse the list of CASE branch constants.
	// the SELECT_CONSTANTS node adopts each constant.
	parse_constant_list(tok, *constants, cs);
	branch->add_child(std::move(branch));

	// look for the : token
	tok = _scanner.current();
	if (tok.type() == &COLON)
	{
		tok = _scanner.next();
	}
	else
	{
		ErrorHandler::flag(tok, &MISSING_COLON, _mp);
	}

	// parse the CASE branch statement. The SELECT_BRANCH node adopts
	// the statement subtree as its second child.
	StatementParser stmnt_parser{ _scanner, _symtabstack, _mp };
	branch->add_child(stmnt_parser.parse(tok));
	return branch;
}

void CaseParser::parse_constant_list(const Token &token, ICodeNode &constant, ConstantSet *cs)
{
	// loop to parse each constant.
	static const auto CONST_START_SET = constant_start_set();
	static const auto COMMA_SET = comma_set();

	auto tok = token;
	while (CONST_START_SET.count(tok.type()))
	{
		// the constants list node adopts the constant node.
		constant.add_child(parse_constant(tok, cs));

		// synchronize at the comma between constants.
		tok = synchronize(COMMA_SET);

		// look for the COMMA
		if (tok.type() == &COMMA)
		{
			tok = _scanner.next(); // consume the ,
		}
		else if (CONST_START_SET.count(tok.type()))
		{
			ErrorHandler::flag(tok, &MISSING_COMMA, _mp);
		}
	}
}

unique_ptr<ICodeNode> CaseParser::parse_constant(const Token &token, ConstantSet *cs)
{
	static const auto CONST_START_SET = constant_start_set();
	const fe::TokenType *sign = nullptr;
	unique_ptr<ICodeNode> constant = nullptr;

	// synchronize at the start of a constant.
	auto tok = token;
	tok = synchronize(CONST_START_SET);
	if (tok.type() == &PLUS || tok.type() == &MINUS)
	{
		sign = tok.type();
		tok = _scanner.next();
	}

	// parse the constant.
	auto type = tok.type();
	if (type == &IDENTIFIER)
	{
		constant = parse_id_constant(tok, sign);
	}
	else if (type == &INTEGER)
	{
		constant = parse_int_constant(tok.lexeme(), sign);
	}
	else if (type == &STRING)
	{
		auto value = tok.value();
		constant = parse_char_constant(tok, boost::apply_visitor(utils::to_str_visitor(), value), sign);
	}
	else
	{
		ErrorHandler::flag(tok, &INVALID_CONSTANT, _mp);
	}

	// check for reused constants
	if (constant)
	{
		auto opt_value = constant->attribute(ICodeKey::VALUE);

		if (!opt_value || cs->count(opt_value.get()))
		{
			ErrorHandler::flag(tok, &CASE_CONSTANT_REUSED, _mp);
		}
		else
		{
			if (opt_value)
			{
				cs->insert(opt_value.get());
			}
		}
	}

	_scanner.next(); // consume the constant
	return constant;
}

unique_ptr<ICodeNode> CaseParser::parse_id_constant(const Token &tok, const psc::fe::TokenType *sign)
{
	// placeholder ! don't allow for now
	ErrorHandler::flag(tok, &INVALID_CONSTANT, _mp);
	return nullptr;
}

unique_ptr<ICodeNode> CaseParser::parse_int_constant(const std::string& value, const psc::fe::TokenType *sign)
{
	auto constant = ICodeFactory::create_node(ICodeNodeType::INTEGER_CONSTANT);
	std::istringstream s(value);

	int val = 0;
	s >> val;

	if (!s && s.eof() == false)
	{
		ErrorHandler::abort_translation(&INVALID_NUMBER, _mp);
	}

	if (sign == &MINUS)
	{
		val = -val;
	}

	constant->set_attribute(ICodeKey::VALUE, val);
	return constant;
}

unique_ptr<ICodeNode> CaseParser::parse_char_constant(const Token &tok, const string &value, const psc::fe::TokenType *sign)
{
	unique_ptr<ICodeNode> constant = nullptr;

	if (sign)
	{
		ErrorHandler::flag(tok, &INVALID_CONSTANT, _mp);
	}
	else
	{
		if (value.size() == 1)
		{
			constant = ICodeFactory::create_node(ICodeNodeType::STRING_CONSTANT);
			constant->set_attribute(ICodeKey::VALUE, value);
		}
		else
		{
			ErrorHandler::flag(tok, &INVALID_CONSTANT, _mp);
		}
	}

	return constant;
}
