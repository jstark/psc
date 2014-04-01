#ifndef _PSC_PASCAL_PARSER_H_
#define _PSC_PASCAL_PARSER_H_

#include <memory>

#include "fe/parser.h"
#include "pascal/scanner.h"
#include "pascal/error_handler.h"
#include "utils/fwd.h"

namespace psc {

FWD_DECL_NS_CL(msg, MessageProducer)
FWD_DECL_NS_CL(msg, ICode)
FWD_DECL_NS_CL(msg, SymbolTableStack)

namespace pascal {

class Parser final : private fe::Parser<pascal::Scanner>
{
public:
    explicit Parser(pascal::Scanner &&scanner, msg::MessageProducer &mp);
    std::tuple<std::unique_ptr<im::ICode>, std::unique_ptr<im::SymbolTableStack>> parse() override;
    int error_count() const override;
private:
    using fe::Parser<pascal::Scanner>::_mp;
};

}}

#endif // _PSC_PASCAL_PARSER_H_

