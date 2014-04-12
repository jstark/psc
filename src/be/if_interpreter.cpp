#include "be/if_interpreter.h"
#include "be/stmnt_interpreter.h"
#include "be/expr_interpreter.h"
#include "be/rt_error.h"
#include "be/rt_error_handler.h"
#include "be/common.h"
#include "im/icode.h"
#include "msg/message.h"

using namespace psc::im;
using namespace psc::be;
using namespace psc::msg;

void* IfInterpreter::execute(const ICodeNode &node, int *exec_count)
{
    // get the IF node's children
    auto children = node.children();
    auto expr_node = children.at(0);
    auto then_node = children.at(1);
    auto else_node = children.size() > 2 ? children.at(2) : nullptr;

    ExprInterpreter expr_interpreter {_mp};
    StatementInterpreter stmnt_intepreter {_mp};

    // evaluate the expression to determine which statement to execute.
    auto b = expr_interpreter.execute(*expr_node, exec_count);
    auto bval = boost::get<bool>(b);

    if (bval)
    {
        stmnt_intepreter.execute(*then_node, exec_count);
    } else
    {
        stmnt_intepreter.execute(*else_node, exec_count);
    }

	++(*exec_count);
    return nullptr;
}
