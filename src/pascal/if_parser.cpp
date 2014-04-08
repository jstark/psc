#include "pascal/if_parser.h"
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
    SynchronizationSet make_then_set()
    {
        auto then = StatementParser::follow_set();
        auto temp = StatementParser::start_set();
        then.insert(temp.begin(), temp.end());
        then.insert(&THEN);
        return then;
    }

    const SynchronizationSet THEN_SET = make_then_set();
}

std::unique_ptr<ICodeNode> IfParser::parse(const Token &current)
{
    auto tok = current; // consume the IF token

    // create an IF node
    auto if_node = ICodeFactory::create_node(ICodeNodeType::IF);

    // parse the expression
    // the IF node adopts the expression subtree as its first child.
    ExprParser expr_parser {_scanner, _symtabstack, _mp };
    if_node->add_child(expr_parser.parse(tok));

    // synchronize
    tok = synchronize(THEN_SET);
    if (tok.type() == &THEN)
    {
        tok = _scanner.next(); // consume the THEN token
    } else
    {
        ErrorHandler::flag(tok, &MISSING_THEN, _mp);
    }

    // parse the THEN statement
    // the IF node adopts the statement subtree as its second child.
    StatementParser stmnt_parser { _scanner, _symtabstack, _mp };
    if_node->add_child(stmnt_parser.parse(tok));

    // look for an ELSE token
    if (tok.type() == &ELSE)
    {
        tok = _scanner.next(); // consume the ELSE token

        // parse the ELSE statement
        // the IF node adopts the statement subtree as its third child.
        if_node->add_child(stmnt_parser.parse(tok));
    }

    return if_node;
}
