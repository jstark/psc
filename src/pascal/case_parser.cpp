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
		return { &IDENTIFIER, &INTEGER, &PLUS, &MINUS, &STRING };
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
	return nullptr;
}

void CaseParser::parse_constant_list(const Token &token, ICodeNode *constant, ConstantSet *cs)
{
	// loop to parse each constant.
	static const auto CONST_START_SET = constant_start_set();
	static const auto COMMA_SET = comma_set();

	auto tok = token;
	while (CONST_START_SET.count(tok.type()))
	{
		// the constants list node adopts the constant node.
		constant->add_child(parse_constant(tok, cs));

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
		//TODO:constant = parse_char_constant(tok, boost::apply_visitor(to_str_visitor(), tok.value()), sign);
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
