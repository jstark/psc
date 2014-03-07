#ifndef _PSC_FE_PARSER_HPP_
#define _PSC_FE_PARSER_HPP_ 

#include "msg/message.h"
#include <tuple>
#include <memory>

namespace psc { 

// forward decl
namespace im {
	class SymbolTableStack;
	class ICode;
}

namespace fe {

class Token;

template<typename S>
class Parser : public msg::MessageProducer
{
public:
    typedef S scanner_type;
    explicit Parser(scanner_type &&scanner)
        : _scanner(std::move(scanner)) {}

    virtual std::tuple<im::ICode*, std::unique_ptr<im::SymbolTableStack>> parse() = 0;
    virtual int error_count() const = 0;

    Token current_token() { return _scanner.current(); }
    Token next_token() { return _scanner.next(); }
private:
	scanner_type _scanner;
};

}}

#endif // _PSC_FE_PARSER_HPP_

