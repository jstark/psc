#include "be/stmnt_interpreter.h"
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
	case ICodeNodeType::NO_OP:
		return nullptr;
	default:
		RuntimeErrorHandler::flag(node, UNIMPLEMENTED_FEATURE, _mp);
		return nullptr;
	}
	return nullptr;
}
