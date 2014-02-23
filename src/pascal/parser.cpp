#include "fe/token.h"
#include "msg/message.h"
#include "pascal/parser.h"
#include "pascal/error.h"
#include "pascal/token_type.h"
#include <chrono>

using namespace psc;
using namespace psc::msg;
using namespace psc::pascal;

Parser::Parser(pascal::Scanner &&scanner)
    : fe::Parser<pascal::Scanner>(std::move(scanner)) {}

void Parser::parse()
{
    fe::Token token;
    auto start = std::chrono::system_clock::now();
    
    try
    {
        do
        {
            token = next_token();
            auto type = token.type();
            if (type != &ERROR)
            {
                Message msg(MessageType::Token,
                            {token.line_number(), token.pos(), type, token.lexeme(), token.value()});
            } else
            {
                _errorHandler.flag(token, boost::any_cast<ErrorCode *>(token.value()), this);
            }
        } while (!token.is_eof());
        
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        
        Message msg(MessageType::ParserSummary, {token.line_number(), error_count(), elapsed.count()});
        send_msg(msg);
    } catch (...)
    {
        _errorHandler.abort_translation(&IO_ERROR, this);
    }
}

int Parser::error_count() const
{
    return _errorHandler.error_count();
}

