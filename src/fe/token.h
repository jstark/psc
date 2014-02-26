#ifndef _PSC_FE_TOKEN_H_
#define _PSC_FE_TOKEN_H_

#include <string>
#include "utils/var.h"

namespace psc { namespace fe {

class Source;


class TokenType
{
public:
    virtual ~TokenType() {}
};

class Token final
{
public:
    friend class TokenBuilder;

    std::string lexeme() const { return _lexeme; }
    int line_number() const { return _lnum; }
    int pos() const { return _pos; }
    const TokenType* type() const { return _type; }
    utils::var value() const { return _value; }
    bool is_eof() const { return _lexeme.empty(); }
private:
    utils::var _value;
    std::string _lexeme;
    int _lnum{0};
    int _pos{0};
    const TokenType *_type{nullptr};
};

class TokenBuilder
{
public:
    TokenBuilder& with_lexeme(const std::string &txt);
    TokenBuilder& with_type(const TokenType *type);
    TokenBuilder& with_value(utils::var v);
    TokenBuilder& at_line(int line_num);
    TokenBuilder& at_pos(int pos);
    Token build();

private:
    Token t;
};

}}

#endif // _PSC_FE_TOKEN_H_

