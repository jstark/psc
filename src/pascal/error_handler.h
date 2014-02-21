#ifndef _PSC_PASCAL_ERROR_HANDLER_H_
#define _PSC_PASCAL_ERROR_HANDLER_H_

#include "fe/token.h"

namespace psc { namespace pascal {

class ErrorCode;
class Parser;

class ErrorHandler 
{
public:
    void flag(const fe::Token &token, const ErrorCode *err, Parser *parser);
    void abort_translation(const ErrorCode *error, Parser *parser);
    int error_count() const { return _errors; }
private:
    static const int MAX_ERRORS = 25;
    static int _errors;
};

}}

#endif // _PSC_PASCAL_ERROR_HANDLER_H_

