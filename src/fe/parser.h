#ifndef _PSC_FE_PARSER_HPP_
#define _PSC_FE_PARSER_HPP_ 

#include "msg/message.h"

namespace psc { namespace fe {

class Token;

template<typename S>
class Parser : msg::MessageProducer
{
public:
    typedef S scanner_type;
    explicit Parser(scanner_type &&scanner)
        : _scanner(std::move(scanner)) {}

    virtual void parse() = 0;
    virtual int error_count() const = 0;

    Token current_token() { return _scanner.current(); }
    Token next_token() { return _scanner.next(); }

    using msg::MessageProducer::add;
    using msg::MessageProducer::send_msg;

private:
    scanner_type _scanner;
};

}}

#endif // _PSC_FE_PARSER_HPP_

