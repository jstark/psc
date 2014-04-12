#include "be/assign_interpreter.h"
#include "be/expr_interpreter.h"
#include "be/rt_error.h"
#include "be/rt_error_handler.h"
#include "be/common.h"
#include "im/icode.h"
#include "msg/message.h"

using namespace psc::im;
using namespace psc::be;
using namespace psc::msg;

namespace
{
    void send_message(const ICodeNode &node, std::string var, ExprVal value, const MessageProducer &mp)
    {
        auto opt_line_num = node.attribute(ICodeKey::LINE);

        // send an ASSIGN message.
        if (opt_line_num)
        {
            auto var_line_num = opt_line_num.get();
            int line_num = boost::get<int>(var_line_num);
			std::string sval = boost::apply_visitor(ExprValToString(), value);
            Message msg(MessageType::Assign, {line_num, var, sval});
            mp.send_msg(msg);
        }
    }
}

void* AssignInterpreter::execute(const ICodeNode &node, int *exec_count)
{
    // the ASSIGN node's children are the target variable
    // and expression
    auto children = node.children();

    auto variable = children.at(0);
    auto expression = children.at(1);

    // Interpret the expression and get its value.
    ExprInterpreter expr_interpreter{ _mp };
    auto value = expr_interpreter.execute(*expression, exec_count);

    // set the value as an attribute of the variable's symbol table entry.
    auto opt_variable_id = variable->attribute(ICodeKey::ID);
    auto var_variable_id = opt_variable_id.get();
    auto entry = boost::get<SymbolTableEntry *>(var_variable_id);

    entry->set_attribute(SymbolTableKey::DataValue, value);

    send_message(node, entry->name(), value, _mp);
    ++(*exec_count);
    return nullptr;
}
