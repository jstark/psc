#ifndef _PSC_BE_COMMON_H_INCLUDED_
#define _PSC_BE_COMMON_H_INCLUDED_

#include "utils/fwd.h"

FWD_DECL_NS_NS_CL(psc, im , ICodeNode)
FWD_DECL_NS_NS_CL(psc, msg, MessageProducer)

namespace psc {
namespace be {

void send_source_line_message(const im::ICodeNode &node, const msg::MessageProducer &mp);

}
}

#endif // _PSC_BE_COMMON_H_INCLUDED_