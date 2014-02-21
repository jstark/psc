#ifndef _PSC_PASCAL_PARSER_H_
#define _PSC_PASCAL_PARSER_H_

#include <memory>

#include "fe/parser.h"
#include "pascal/scanner.h"

namespace psc { namespace pascal {

class Parser final : fe::Parser<pascal::Scanner>
{
public:
    explicit Parser(pascal::Scanner &&scanner);
    void parse() override;
    int error_count() const override;

    using fe::Parser<pascal::Scanner>::add;
    using fe::Parser<pascal::Scanner>::send_msg;
};

}}

#endif // _PSC_PASCAL_PARSER_H_

