#ifndef _PSC_PASCAL_ERROR_HANDLER_H_
#define _PSC_PASCAL_ERROR_HANDLER_H_

#include "fe/token.h"
#include "msg/message.h"

namespace psc { namespace pascal {

class ErrorCode;
class Parser;

class ErrorHandler 
{
public:
    static void flag(const fe::Token &token, const ErrorCode *err, msg::MessageProducer *mp);
    static void abort_translation(const ErrorCode *error, msg::MessageProducer *mp);
    static int error_count() { return _errors; }
private:
    static const int MAX_ERRORS = 25;
    static int _errors;
};

}}

#endif // _PSC_PASCAL_ERROR_HANDLER_H_

