#include "fe/source.h"
#include "fe/token.h"
#include "fe/scanner.h"

using namespace psc::fe;

struct Scanner::ScannerImpl
{
public:
    explicit ScannerImpl(Source &&source)
        : _source(std::move(source)) {}

    Source _source;
    Token _token;
};

Scanner::Scanner(Source &&src)
    : _pimpl(new Scanner::ScannerImpl(std::move(src))) {}

Scanner::~Scanner() {}

Scanner::Scanner(Scanner &&rhs)
    : _pimpl(std::move(rhs._pimpl)) {}

Token Scanner::current() const
{
    return _pimpl->_token;
}

Token Scanner::next()
{
    _pimpl->_token = extract_token();
    return _pimpl->_token;
}

char Scanner::current_char()
{
    return _pimpl->_source.current();
}

char Scanner::next_char()
{
    return _pimpl->_source.next();
}

Source& Scanner::the_source()
{
    return _pimpl->_source;
}
