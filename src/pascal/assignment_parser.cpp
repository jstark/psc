#include "pascal/assignment_parser.h"
#include "pascal/stmnt_parser.h"
#include "pascal/scanner.h"
#include "pascal/token_type.h"
#include "pascal/error.h"
#include "fe/token.h"

using namespace psc::fe;
using namespace psc::im;
using namespace psc::pascal;

namespace
{

}

std::unique_ptr<ICodeNode> AssignmentParser::parse(const Token &current)
{
	// TODO
	return nullptr;
}
