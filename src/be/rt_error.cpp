#include "be/rt_error.h"

using namespace psc::be;

RuntimeErrorCode::RuntimeErrorCode(const std::string &msg)
: _message(msg) {}

namespace psc {
	namespace pascal {

#define DEFINE_ERROR_CODE(var, msg) \
	const RuntimeErrorCode var{ msg };

#define X DEFINE_ERROR_CODE
		RTCODES
#undef X

}
}