#include "pascal/expr_parser.h"
#include "pascal/scanner.h"
#include "pascal/token_type.h"
#include "pascal/error.h"
#include "fe/token.h"
#include <unordered_map>

using namespace psc::fe;
using namespace psc::im;
using namespace psc::pascal;

namespace
{

using OpsMap = std::unordered_map<const psc::fe::TokenType *, ICodeNodeType>;

const SynchronizationSet EXPR_START = { &PLUS, &MINUS, &IDENTIFIER, &INTEGER, &REAL, &STRING, &NOT, &LEFT_PAREN };

const SynchronizationSet REL_OPS = { &EQUALS, &NOT_EQUALS, &LESS_THAN, &LESS_EQUALS, &GREATER_THAN, &GREATER_EQUALS };

const OpsMap REL_OPS_MAP = {
	{ &EQUALS, ICodeNodeType::EQ },
	{ &NOT_EQUALS, ICodeNodeType::NE },
	{ &LESS_THAN, ICodeNodeType::LT },
	{ &LESS_EQUALS, ICodeNodeType::LE },
	{ &GREATER_THAN, ICodeNodeType::GT },
	{ &GREATER_EQUALS, ICodeNodeType::GE }
};

const SynchronizationSet ADD_OPS = { &PLUS, &MINUS, &OR };

const OpsMap ADD_OPS_MAP = {
	{ &PLUS, ICodeNodeType::ADD },
	{ &MINUS, ICodeNodeType::SUBTRACT },
	{ &OR, ICodeNodeType::OR }
};

const SynchronizationSet MUL_OPS = { &STAR, &SLASH, &DIV, &MOD, &AND };

const OpsMap MUL_OPS_MAP = {
	{ &STAR, ICodeNodeType::MULTIPLY },
	{ &SLASH, ICodeNodeType::FLOAT_DIVIDE },
	{ &DIV, ICodeNodeType::INTEGER_DIVIDE },
	{ &MOD, ICodeNodeType::MOD },
	{ &AND, ICodeNodeType::AND },
};

}

std::unique_ptr<ICodeNode> ExprParser::parse(const Token &current)
{
	return nullptr;
}
