#include "fe/token.h"
#include "fe/source.h"

using std::string;
using namespace psc::fe;
using boost::any;

Token TokenExtractor::extract(Source &s)
{
    TokenBuilder builder;

    auto c = s.current();
    if (c != Source::END_OF_FILE)
    {
        builder.with_lexeme(string(1, c));
    }
    builder.at_line(s.current_line());
    builder.at_pos(s.current_pos());
    s.next();
    return builder.build();
}

TokenBuilder& TokenBuilder::with_lexeme(const string &txt)
{
    t._lexeme = txt;
    return *this;
}

TokenBuilder& TokenBuilder::with_type(const TokenType *type)
{
    t._type = type;
    return *this;
}

TokenBuilder& TokenBuilder::with_value(any v)
{
    t._value = v;
    return *this;
}

TokenBuilder& TokenBuilder::at_line(int line_num)
{
    t._lnum = line_num;
    return *this;
}

TokenBuilder& TokenBuilder::at_pos(int pos)
{
    t._pos = pos;
    return *this;
}

Token TokenBuilder::build()
{
    Token v;
    std::swap(v, t);
    return v;
}




