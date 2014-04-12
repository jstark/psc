#include "be/rt_error_handler.h"
#include "be/rt_error.h"
#include "im/icode.h"
#include "msg/message.h"

using namespace psc::im;
using namespace psc::be;
using namespace psc::msg;

namespace
{
	bool no_line_attribute(const ICodeNode *node)
	{
		auto line = node->attribute(ICodeKey::LINE);
		return !line;
	}
}

int RuntimeErrorHandler::_errors = 0;

void RuntimeErrorHandler::flag(const ICodeNode &n, const RuntimeErrorCode &error, const MessageProducer &mp)
{
	const ICodeNode *node = &n;
	while (node != nullptr && no_line_attribute(node))
	{
		node = node->parent();
	}

	auto opt_line = node->attribute(ICodeKey::LINE);
	auto var_line = opt_line.get();
	// notify observers
	Message msg(MessageType::RuntimeError, { error.text(), boost::get<int>(var_line) });
	mp.send_msg(msg);

	if (++_errors > MAX_ERRORS)
	{
		std::string error_msg("*** ABORTED AFTER TOO MANY RUNTIMER ERRORS ***");
		Message abort_msg(MessageType::RuntimeError, { error_msg, 0 });
		mp.send_msg(abort_msg);
		std::exit(-1);
	}
}
