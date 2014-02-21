#ifndef _PSC_FE_SCANNER_H_
#define _PSC_FE_SCANNER_H_

#include <memory>

namespace psc { namespace fe {

class Source;
class Token;

class Scanner
{
public:
    explicit Scanner(Source &&source);
    Scanner(Scanner &&rhs);
    virtual ~Scanner();

    Token current() const;
    Token next();

    // Source interface
    char current_char();
    char next_char();

protected:
    Source& the_source();
    virtual Token extract_token() = 0;

private:
    struct ScannerImpl;
    std::unique_ptr<ScannerImpl> _pimpl;
};

}}

#endif // _PSC_FE_SCANNER_H_ 

