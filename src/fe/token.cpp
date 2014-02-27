#include "fe/token.h"
#include "fe/source.h"

using std::string;
using namespace psc::fe;

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

TokenBuilder& TokenBuilder::with_value(psc::utils::var v)
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




