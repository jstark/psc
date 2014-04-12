#ifndef _PSC_PASCAL_CASE_PARSER_H_INCLUDED_
#define _PSC_PASCAL_CASE_PARSER_H_INCLUDED_

#include "im/icode.h"
#include "im/common.h"
#include "utils/fwd.h"
#include "pascal/nt_parser.h"
#include <memory>
#include <string>
#include <unordered_set>
#include <boost/functional/hash.hpp>
#include <boost/variant.hpp>

FWD_DECL_NS_NS_CL(psc, fe, Token)
FWD_DECL_NS_NS_CL(psc, fe, TokenType)
FWD_DECL_NS_NS_CL(psc, im, SymbolTableStack)
FWD_DECL_NS_NS_CL(psc, msg, MessageProducer)

namespace psc {
namespace pascal {

class Scanner;

using ConstantSet = std::unordered_set<psc::im::ICodeNodeAttrValue, boost::hash<psc::im::ICodeNodeAttrValue>>;

class CaseParser : public NonTerminalParser
{
public:
	CaseParser(Scanner &s, im::SymbolTableStack &ss, const msg::MessageProducer &mp)
		: NonTerminalParser(s, ss, mp) {}

	std::unique_ptr<im::ICodeNode> parse(const fe::Token &current);
private:
	std::unique_ptr<im::ICodeNode> parse_branch(const fe::Token &token, ConstantSet *cs);
	void parse_constant_list(const fe::Token &token, im::ICodeNode &constant, ConstantSet *cs);
	std::unique_ptr<im::ICodeNode> parse_constant(const fe::Token &tok, ConstantSet *cs);
	std::unique_ptr<im::ICodeNode> parse_id_constant(const fe::Token &tok, const fe::TokenType *sign);
	std::unique_ptr<im::ICodeNode> parse_int_constant(const std::string& value, const fe::TokenType *sign);
	std::unique_ptr<im::ICodeNode> parse_char_constant(const fe::Token &tok, const std::string &value, const fe::TokenType *sign);
};

}
}

#endif // _PSC_PASCAL_CASE_PARSER_H_INCLUDED_
