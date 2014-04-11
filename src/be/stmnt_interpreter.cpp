#include "be/stmnt_interpreter.h"
#include "be/assign_interpreter.h"
#include "be/compound_interpreter.h"
#include "be/if_interpreter.h"
#include "be/loop_interpreter.h"
#include "be/case_interpreter.h"
#include "be/rt_error.h"
#include "be/rt_error_handler.h"
#include "be/common.h"
#include "im/icode.h"
#include "msg/message.h"

using namespace psc::im;
using namespace psc::be;
using namespace psc::msg;

void* StatementInterpreter::execute(const ICodeNode &node, int *exec_count)
{
	// send a message about the current source line
	send_source_line_message(node, _mp);

	auto type = node.type();
	switch (type)
	{
	case ICodeNodeType::COMPOUND:
	{
		CompoundInterpreter compound_interpreter{ _mp };
		return compound_interpreter.execute(node, exec_count);
	}
    case ICodeNodeType::ASSIGN:
    {
        AssignInterpreter assign_interpreter{ _mp };
        return assign_interpreter.execute(node, exec_count);
    }
    case ICodeNodeType::IF:
    {
        IfInterpreter if_interpreter{ _mp };
        return if_interpreter.execute(node, exec_count);
    }
    case ICodeNodeType::LOOP:
    {
        LoopInterpreter loop_interpreter{ _mp };
        return loop_interpreter.execute(node, exec_count);
    }
    case ICodeNodeType::SELECT:
    {
        CaseInterpreter case_interpreter{_mp };
        return case_interpreter.execute(node, exec_count);
    }
	case ICodeNodeType::NO_OP:
		return nullptr;
	default:
		RuntimeErrorHandler::flag(node, UNIMPLEMENTED_FEATURE, _mp);
		return nullptr;
	}
	return nullptr;
}
