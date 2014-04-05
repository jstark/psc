#include "pascal/expr_parser.h"
#include "pascal/scanner.h"
#include "pascal/token_type.h"
#include "pascal/error.h"
#include "pascal/error_handler.h"
#include "im/symbol_table.h"
#include "fe/token.h"
#include <unordered_map>
#include <boost/variant.hpp>

using namespace psc::fe;
using namespace psc::im;
using namespace psc::pascal;

namespace psc {
namespace pascal {


const SynchronizationSet EXPR_START = { &PLUS, &MINUS, &IDENTIFIER, &INTEGER, &REAL, &STRING, &NOT, &LEFT_PAREN };

}
}

namespace
{

using OpsMap = std::unordered_map<const psc::fe::TokenType *, ICodeNodeType>;

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

class to_val_visitor : public boost::static_visitor<ICodeNodeAttrValue>
{
public:
    ICodeNodeAttrValue operator()(int i) const
    {
        return i;
    }

    ICodeNodeAttrValue operator()(double d) const
    {
        return d;
    }

    ICodeNodeAttrValue operator()(const std::string& s) const
    {
        return s;
    }

    ICodeNodeAttrValue operator()(const void *p) const
    {
        return "__ptr__";
    }
};

}

std::unique_ptr<ICodeNode> ExprParser::parse(const Token &current)
{
    return parse_expression(current);
}

std::unique_ptr<ICodeNode> ExprParser::parse_expression(const Token &current)
{
    // parse a simple expression, and make it the root
    auto tok = current;
    auto root = parse_simple_expression(tok);
    tok = _scanner.current();
    auto type = tok.type();

    if (REL_OPS.count(type))
    {
        // cteate a new operator node and adopt the current tree
        // as its first child.
        auto op_type = REL_OPS_MAP.at(type);
        auto op_node = ICodeFactory::create_node(op_type);
        op_node->add_child(std::move(root));

        tok = _scanner.next(); // consume the operator

        // parse the second simple expression. the operator node
        // adopts the simple expression's tree as its second child.
        op_node->add_child(parse_simple_expression(tok));

        // the operator node becomes the new node
        root = std::move(op_node);
    }
    return root;
}

std::unique_ptr<ICodeNode> ExprParser::parse_simple_expression(const Token &current)
{
    const fe::TokenType *sign = nullptr; // type of leading sign, if any

    // look for a leading sign + or -
    auto tok = current;
    auto type = tok.type();
    if (type == &PLUS || type == &MINUS)
    {
        sign = type;
        tok = _scanner.next(); // consume + or -
    }

    // parse a term and make the root of its tree the root node
    auto root = parse_term(tok);

    // was there a leading - sign ?
    if (sign == &MINUS)
    {
        // create a NEGATE node and adopt the current tree
        // as its child. the NEGATE node becomes the new root node.
        auto negate = ICodeFactory::create_node(ICodeNodeType::NEGATE);
        negate->add_child(std::move(root));
        root = std::move(negate);
    }

    tok = _scanner.current();
    type = tok.type();

    // loop over additive operators
    while (ADD_OPS.count(type))
    {
        // create a new operator node and adopt the current tree as its
        // first child.
        auto op_type = ADD_OPS_MAP.at(type);
        auto op_node = ICodeFactory::create_node(op_type);
        op_node->add_child(std::move(root));

        tok = _scanner.next();

        // parse another term. The operator node adopts
        // the term's tree as its second child.
        op_node->add_child(parse_term(tok));

        // the op_node becomes the new root node
        root = std::move(op_node);
        tok = _scanner.current();
        type = tok.type();
    }

    return root;
}

std::unique_ptr<ICodeNode> ExprParser::parse_term(const Token &current)
{
    // parse a factor and make its node the root node.
    auto tok = current;
    auto root = parse_factor(tok);
    tok = _scanner.current();
    auto type = tok.type();

    while (MUL_OPS.count(type))
    {
        // create a new operator node and adopt the current tree
        // as its first child.
        auto op_type = MUL_OPS_MAP.at(type);
        auto op_node = ICodeFactory::create_node(op_type);
        op_node->add_child(std::move(root));

        tok = _scanner.next(); // consume the operator

        // parse another factor. The operator node adopts
        // the term's tree as its second child.
        op_node->add_child(parse_factor(tok));

        // the operator node becomes the new root node.
        root = std::move(op_node);
        tok = _scanner.current();
        type = tok.type();
    }
    return root;
}

std::unique_ptr<ICodeNode> ExprParser::parse_factor(const Token &current)
{
    std::unique_ptr<ICodeNode> root = nullptr;
    auto tok = current;
    auto type= tok.type();
    if (type == &IDENTIFIER)
    {
        // look the identifier in the symboltablestack.
        // flag the identifier as undefined if it's not found.
        auto name = tok.lexeme();
        auto opt_id = _symtabstack.lookup(name);
        auto id = opt_id.get_value_or(nullptr);
        if (id == nullptr)
        {
            ErrorHandler::flag(tok, &IDENTIFIER_UNDEFINED, _mp);
            id = _symtabstack.enter_local(name);
        }

        root = ICodeFactory::create_node(ICodeNodeType::VARIABLE);
        root->set_attribute(ICodeKey::ID, id);
        id->append_line(tok.line_number());
        _scanner.next();

    } else if (type == &INTEGER)
    {
        // create a INTEGER constant node as the root node.
        root = ICodeFactory::create_node(ICodeNodeType::INTEGER_CONSTANT);
        auto tokval = tok.value();
        root->set_attribute(ICodeKey::VALUE, boost::apply_visitor(to_val_visitor(), tokval));
        tok = _scanner.next();
    } else if (type == &REAL)
    {

        // create a REAL constant node as the root node.
        root = ICodeFactory::create_node(ICodeNodeType::REAL_CONSTANT);
        auto tokval = tok.value();
        root->set_attribute(ICodeKey::VALUE, boost::apply_visitor(to_val_visitor(), tokval));
        tok = _scanner.next();
    } else if (type == &STRING)
    {
        // create a STRING constant node as the root node.
        root = ICodeFactory::create_node(ICodeNodeType::STRING_CONSTANT);
        auto tokval = tok.value();
        root->set_attribute(ICodeKey::VALUE, boost::apply_visitor(to_val_visitor(), tokval));
        tok = _scanner.next();
    } else if (type == &NOT)
    {
        tok = _scanner.next(); // consume NOT

        // create a NOT node as the root node
        root = ICodeFactory::create_node(ICodeNodeType::NOT);

        // parse a factor. The NOT node adopts the factor
        // as its child.
        root->add_child(parse_factor(tok));
    } else if (type == &LEFT_PAREN)
    {
        tok = _scanner.next(); // consume the '('

        // parse an expression and make its node the root node
        root = parse_expression(tok);

        // look for the matching ')' token
        tok = _scanner.current();
        if (tok.type() == &RIGHT_PAREN)
        {
            _scanner.next(); // consume the ')'
        }
    } else
    {
        ErrorHandler::flag(tok, &UNEXPECTED_TOKEN, _mp);
    }

    return root;
}
