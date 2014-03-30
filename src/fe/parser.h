#ifndef _PSC_FE_PARSER_HPP_
#define _PSC_FE_PARSER_HPP_ 

#include <tuple>
#include <memory>
#include "utils/fwd.h"

namespace psc { 

FWD_DECL_NS_CL(im, SymbolTableStack)
FWD_DECL_NS_CL(im, ICode)
FWD_DECL_NS_CL(msg, MessageProducer)

namespace fe {

class Token;

template<typename S>
class Parser
{
public:
    typedef S scanner_type;
    explicit Parser(scanner_type &&scanner, msg::MessageProducer &mp)
        : _scanner(std::move(scanner)), _mp(mp) {}

    virtual std::tuple<im::ICode*, std::unique_ptr<im::SymbolTableStack>> parse() = 0;
    virtual int error_count() const = 0;

    Token current_token() { return _scanner.current(); }
    Token next_token() { return _scanner.next(); }

	scanner_type _scanner;
    msg::MessageProducer &_mp;
};

}}

#endif // _PSC_FE_PARSER_HPP_

