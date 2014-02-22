#include "pascal/error_handler.h"
#include "pascal/error.h"
#include "pascal/parser.h"
#include "msg/message.h"
#include <vector>

using namespace psc::pascal;
using namespace psc::msg;

int ErrorHandler::_errors = 0;

void ErrorHandler::flag(const psc::fe::Token& tok, const ErrorCode *err, Parser *parser)
{
    Message msg(MessageType::SyntaxError, 
            {tok.line_number(), tok.pos(), tok.lexeme(), err->text()});
    parser->send_msg(msg);

    if (++_errors > MAX_ERRORS)
    {
        abort_translation(&TOO_MANY_ERRORS, parser);
    }
}

void ErrorHandler::abort_translation(const ErrorCode *error, Parser *parser)
{
    using std::vector;
    using std::string;

    string fatal = "FATAL ERROR: " + error->text();
    Message msg(MessageType::SyntaxError, {0, 0, std::string(""), fatal});
    parser->send_msg(msg);
}

