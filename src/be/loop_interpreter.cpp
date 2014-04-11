#include "be/loop_interpreter.h"
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

void* LoopInterpreter::execute(const ICodeNode &node, int *exec_count)
{
    bool exit_loop = false;
    ICodeNode *expr_node = nullptr;
    auto loop_children = node.children();

    ExprInterpreter expr_interpreter{_mp};
    StatementInterpreter stmtn_interpreter{_mp};

    // loop until the TEST expression value is true
    while (!exit_loop)
    {
        ++(*exec_count);

        // execute the children of the LOOP node
        for (auto ch : loop_children)
        {
            if (ch->type() == ICodeNodeType::TEST)
            {
                if (expr_node == nullptr)
                {
                    expr_node = ch->children().at(0);
                }
                auto b = expr_interpreter.execute(*expr_node, exec_count);
                exit_loop = boost::get<bool>(b);
            } else
            {
                stmtn_interpreter.execute(*ch, exec_count);
            }

            if (exit_loop)
            {
                break;
            }
        }
    }
    return nullptr;
}
