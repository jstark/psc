#ifndef _PSC_PASCAL_SCANNER_H_
#define _PSC_PASCAL_SCANNER_H_

#include "fe/scanner.h"
#include "fe/source.h"
#include "fe/token.h"

namespace psc { namespace pascal {

class Scanner : public fe::Scanner
{
public:
    explicit Scanner(fe::Source &&source);
    Scanner(Scanner &&rhs) = default;

private:
    fe::Token extract_token() override;
    void skipws();
};

}}

#endif // _PSC_PASCAL_SCANNER_H_
