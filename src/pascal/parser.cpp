#include "fe/token.h"
#include "msg/message.h"
#include "pascal/parser.h"
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

    do 
    {
        token = next_token();
    } while (!token.is_eof());

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    Message msg(MessageType::ParserSummary, {token.line_number(), error_count(), elapsed.count()});
    send_msg(msg);
}

int Parser::error_count() const
{
    return 0;
}

