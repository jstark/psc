#include "be/common.h"
#include "im/icode.h"
#include "msg/message.h"

using namespace psc::im;
using namespace psc::be;
using namespace psc::msg;


void psc::be::send_source_line_message(const ICodeNode &node, const MessageProducer &mp)
{
	auto opt_line_number = node.attribute(ICodeKey::LINE);

	// send the message
	if (opt_line_number)
	{
		auto line_number_val = opt_line_number.get();
		auto line_number = boost::get<int>(line_number_val);
		Message msg(MessageType::SourceLine, { line_number });
		mp.send_msg(msg);
	}
}
