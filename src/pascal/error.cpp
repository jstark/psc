#include "pascal/error.h"

using namespace psc::pascal;

static int get_next_id()
{
    static int id = 0;
    return id++;
}

ErrorCode::ErrorCode(const std::string &msg)
    : _id(get_next_id()), _message(msg) {}

ErrorCode::ErrorCode(int code, const std::string &msg)
    : _id(code), _message(msg) {}

namespace psc { namespace pascal {

#define DEFINE_ERROR_CODE(var, msg) \
    const ErrorCode var{msg};

#define DEFINE_FATAL_ERROR_CODE(var, code, msg) \
    const ErrorCode var{code, msg};

#define X DEFINE_ERROR_CODE
CODES
#undef X

#define X DEFINE_FATAL_ERROR_CODE
FATAL_CODES
#undef X

}}
