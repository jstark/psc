#include "be/compound_interpreter.h"
#include "be/stmnt_interpreter.h"
#include "be/rt_error.h"
#include "be/rt_error_handler.h"
#include "be/common.h"
#include "im/icode.h"
#include "msg/message.h"

using namespace psc::im;
using namespace psc::be;
using namespace psc::msg;

void* CompoundInterpreter::execute(const ICodeNode &node, int *exec_count)
{
	// send a message about the current source line
	StatementInterpreter stmnt_interpreter{ _mp };
	for (const auto &child : node.children())
	{
		stmnt_interpreter.execute(*child, exec_count);
		(*exec_count)++;
	}
	return nullptr;
}