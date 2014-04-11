#include "be/case_interpreter.h"
#include "be/expr_interpreter.h"
#include "be/stmnt_interpreter.h"
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
    bool search_constants(CaseConstant val, const ICodeNode &child)
    {
        // are the values integer or string ?
        bool integer_mode = val.which() == 0;

        // get the list of SELECT_CONSTANTS values
        auto constants = child.children().at(0);
        auto all_constants = constants->children();

        // search the list of constants
        if (integer_mode)
        {
            int v = boost::get<int>(val);
            for (auto constant : all_constants)
            {
                auto opt_con = constant->attribute(ICodeKey::VALUE);
                auto var_con = opt_con.get();
                int q = boost::get<int>(var_con);
                if (v == q)
                {
                    return true;
                }
            }
        } else
        {
            std::string v = boost::get<std::string>(val);
            for (auto constant : all_constants)
            {
                auto opt_con = constant->attribute(ICodeKey::VALUE);
                auto var_con = opt_con.get();
                std::string q = boost::get<std::string>(var_con);
                if (v == q)
                {
                    return true;
                }
            }
        }
        return false;
    }

    template<typename T>
    ICodeNode* search_branches(CaseConstant val, T begin, T end)
    {
        // loop over the SELECT_BRANCHes to find a match
        for ( ; begin != end; ++begin)
        {
            auto child = *begin;
            if (search_constants(val, child))
            {
                return child;
            }
        }
        return nullptr;
    }
}

void* CaseInterpreter::execute(const ICodeNode &node, int *exec_count)
{
    // get the SELECT node's children
    auto children = node.children();
    auto expr_node = children.at(0);

    // evaluate SELECT expression
    ExprInterpreter expr_interpreter{_mp};
    auto value = expr_interpreter.execute(*expr_node, exec_count);

    // attempt to select a SELECT_BRANCH
    // TODO
    ICodeNode *selected_branch = nullptr;

    // if the was a selection, execute the SELECT_BRANCH's statements
    if (selected_branch)
    {
        auto stmnt_node = selected_branch->children().at(1);
        StatementInterpreter stmnt_interpreter{_mp};
        stmnt_interpreter.execute(*stmnt_node, exec_count);
    }

    ++(*exec_count);
    return nullptr;
}
